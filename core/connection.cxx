#ifdef WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <SDKDDKVer.h>
#endif

#include "connection.hxx"

#include <boost/circular_buffer.hpp>
#include <libssh/callbacks.h>

#define commlog(str, ...) log(str, LOG_COMM, sysdata.log_level, ##__VA_ARGS__)

class TelnetConnection : public Connection
{
  private:
    boost::asio::awaitable<void> readSome()
    {
        std::array<char, MAX_INPUT_LENGTH> readBuffer;
        assert(m_state == State::Open);
        assert(m_waitingToRead == false);

        m_waitingToRead = true;

        size_t bytesRead = co_await m_socket->async_read_some(boost::asio::buffer(&readBuffer[0], readBuffer.size()),
                                                              boost::asio::use_awaitable);

        // TODO error handling
        // TODO what happens if we overflow m_inputBuffer
        m_inputBuffer.insert(m_inputBuffer.end(), &readBuffer[0], &readBuffer[bytesRead]);

        m_waitingToRead = false;
    }

  public:
    TelnetConnection(IOManager& ioManager, std::unique_ptr<boost::asio::ip::tcp::socket> socket)
        : Connection(ioManager, std::move(socket))
    {
    }

    boost::asio::awaitable<std::string> readLine() override
    {
        size_t lineEnd = 0;

        while (true)
        {
            for (; lineEnd < m_inputBuffer.size(); lineEnd++)
            {
                char c = m_inputBuffer[lineEnd];
                if (c == '\n' || c == '\r')
                    break;

                assert(c != '\0');
            }

            if (lineEnd == m_inputBuffer.size())
            {
                co_await readSome();
                continue;
            }

            std::vector<char> stringBuf(lineEnd + 2, '\0');

            // old telnet clients still like to send the backspace character - handle it here
            size_t outputPos = 0;
            for (size_t inputPos = 0; inputPos < lineEnd; inputPos++)
            {
                char inputChar = m_inputBuffer[inputPos];

                if (inputChar == '\b' && outputPos > 0)
                    outputPos--;
                else if (isascii(inputChar) && isprint(inputChar))
                    stringBuf[outputPos++] = inputChar;
            }

            if (outputPos == 0)
            {
                stringBuf[outputPos] = ' ';
                outputPos++;
            }

            stringBuf[outputPos] = '\0';
            // TODO repeat protection, '!' last command substitution need to be done on the engine side

            std::string result(&stringBuf[0]);

            while (m_inputBuffer[lineEnd] == '\n' || m_inputBuffer[lineEnd] == '\r')
                lineEnd++;

            m_inputBuffer.erase_begin(lineEnd);

            co_return result;
        }
    }

    boost::asio::awaitable<void> flushOutput() override
    {
        std::array<char, MAX_OUTPUT_SIZE> writeBuffer;

        assert(m_state != State::Closed);
        assert(m_flushState != FlushState::Running);

        m_flushState = FlushState::Running;

        if (m_outputBuffer.empty())
        {
            bug("flushOutput triggered on connection with nothing to flush");
        }

        while (!m_outputBuffer.empty())
        {
            const size_t bytesToSend = std::min(writeBuffer.size(), m_outputBuffer.size());

            for (size_t i = 0; i < bytesToSend; i++)
            {
                writeBuffer[i] = m_outputBuffer[i];
            }

            m_outputBuffer.erase_begin(bytesToSend);

            // TODO error handling
            const size_t written = co_await m_socket->async_write_some(
                boost::asio::buffer(&writeBuffer[0], writeBuffer.size()), boost::asio::use_awaitable);

            m_outputBuffer.erase_begin(written);
        }

        m_flushState = FlushState::Stopped;
    }

    boost::asio::awaitable<void> close() override
    {
        m_socket->close();

        co_return;
    }
};

class SshConnection : public Connection
{
  private:
    ssh_server_callbacks_struct m_callbacks = {};
    ssh_session m_session = nullptr;
    ssh_event m_loop = nullptr;
    ssh_channel m_channel = nullptr;
    ssh_channel_callbacks_struct m_channelCallbacks = {};
    std::array<char, MAX_OUTPUT_SIZE> m_writeBuffer;
    std::string m_authenticatedUsername;
    bool m_shellRequested = false;

    boost::asio::awaitable<void> runKeyExchange()
    {
        assert(!m_waitingToRead);

        m_waitingToRead = true;
        m_flushState = FlushState::Running;

        co_await m_socket->async_wait(boost::asio::ip::tcp::socket::wait_write, boost::asio::use_awaitable);

        while (true)
        {
            co_await m_socket->async_wait(boost::asio::ip::tcp::socket::wait_read, boost::asio::use_awaitable);

            // TODO error handling - this could throw

            int ret = ssh_handle_key_exchange(m_session);

            if (ret == SSH_AGAIN)
            {
                commlog("continuing SSH key exchange...");

                continue;
            }
            else if (ret == SSH_OK)
            {

                assert(ret == SSH_OK);

                commlog("completed key exchange");

                ssh_set_auth_methods(m_session, SSH_AUTH_METHOD_PASSWORD | SSH_AUTH_METHOD_PUBLICKEY);
                ret = ssh_event_add_session(m_loop, m_session);
                assert(ret == SSH_OK);

                m_waitingToRead = false;
                m_flushState = FlushState::Stopped;

                while (!m_shellRequested)
                {
                    int pollFlags = ssh_get_poll_flags(m_session);

                    if (pollFlags & SSH_WRITE_PENDING)
                    {
                        co_await doIO(IOType::Write);
                    }
                    else
                    {
                        assert(pollFlags & SSH_READ_PENDING);
                        co_await doIO(IOType::Read);
                    }
                }

                commlog("client requested a shell - notifying the game");

                notifyGameAuthenticatedUserConnected(m_authenticatedUsername);
                co_return;
            }
            else
            {
                assert(ret == SSH_ERROR);
                commlog("SSH key exchange failed: %s", ssh_get_error(m_session));
                commlog("SSH error code: %d", ssh_get_error_code(m_session));
                close();
                co_return;
            }
        }

        m_waitingToRead = false;
        m_flushState = FlushState::Stopped;
    }

    enum class IOType
    {
        Read,
        Write
    };

    boost::asio::awaitable<void> doIO(IOType loopType)
    {
        if (loopType == IOType::Read)
        {
            assert(!m_waitingToRead);
            m_waitingToRead = true;
        }

        const auto waitType = (loopType == IOType::Read) ? boost::asio::ip::tcp::socket::wait_read
                                                         : boost::asio::ip::tcp::socket::wait_write;

        co_await m_socket->async_wait(waitType, boost::asio::use_awaitable);

        int ret = ssh_event_dopoll(m_loop, 0);

        if (loopType == IOType::Read)
            m_waitingToRead = false;

        if (ret == SSH_ERROR)
        {
            commlog("SSH connection error: %s", ssh_get_error(m_session));
            commlog("SSH error code: %d", ssh_get_error_code(m_session));
            // TODO tear down the socket
            throw std::runtime_error("IO error on SSH socket");
            co_return;
        }
    }

    int sendBanner()
    {
        ssh_set_auth_methods(m_session, SSH_AUTH_METHOD_PASSWORD | SSH_AUTH_METHOD_PUBLICKEY);

        std::string banner = m_manager.getBanner();

        ssh_string sshBanner = ssh_string_from_char(banner.c_str());
        int ret = ssh_send_issue_banner(m_session, sshBanner);
        ssh_string_free(sshBanner);
        assert(ret == SSH_OK);

        return SSH_AUTH_DENIED;
    }

    int handlePasswordAuth(const char* user, const char* password)
    {
        if (requestPasswordAuth(user, password))
        {
            m_authenticatedUsername = user;
            return SSH_AUTH_SUCCESS;
        }
        return SSH_AUTH_DENIED;
    }

    int handlePubkeyAuth(const char* user, ssh_key pubkey, char signatureState)
    {
        if (signatureState != SSH_PUBLICKEY_STATE_NONE && signatureState != SSH_PUBLICKEY_STATE_VALID)
            return SSH_AUTH_DENIED;

        auto pubkeys = getPubkeysForUser(user);

        for (const auto& key : pubkeys)
        {
            auto keyType = ssh_key_type_from_name(key.type.c_str());

            if (keyType == SSH_KEYTYPE_UNKNOWN)
            {
                bug("unknown key type: %s", key.type.c_str());
                continue;
            }

            ssh_key knownKey = nullptr;

            int ret = ssh_pki_import_pubkey_base64(key.key.c_str(), keyType, &knownKey);

            if (ret != SSH_OK)
            {
                bug("failed to import pubkey: %s %s", key.type.c_str(), key.key.c_str());
                continue;
            }

            ret = ssh_key_cmp(pubkey, knownKey, SSH_KEY_CMP_PUBLIC);

            ssh_key_free(knownKey);

            if (ret == SSH_OK)
            {
                m_authenticatedUsername = user;
                return SSH_AUTH_SUCCESS;
            }
        }

        return SSH_AUTH_DENIED;
    }

    ssh_channel openChannel()
    {
        commlog("opening a channel");

        if (m_channel != nullptr)
        {
            commlog("SSH client tried to open a second channel");
            return nullptr;
        }

        m_channel = ssh_channel_new(m_session);
        ssh_set_channel_callbacks(m_channel, &m_channelCallbacks);

        return m_channel;
    }

    int requestPty(const char* term, int x, int y, int px, int py)
    {
        commlog("PTY requested: %s %d %d %d %d", term, x, y, px, py);
        // notifyGameWindowSizeChanged(x, y);
        return 0;
    }

    int updatePty(int x, int y, int px, int py)
    {
        // notifyGameWindowSizeChanged(x, y);
        return 0;
    }

    int requestShell()
    {
        commlog("shell requested");
        commlog("ssh channel has window %d", ssh_channel_window_size(m_channel));

        if (m_authenticatedUsername.empty())
        {
            assert(!m_authenticatedUsername.empty()); // libssh shouldn't let this happen
            return SSH_ERROR;
        }

        m_shellRequested = true;
        return 0;
    }

    boost::asio::awaitable<void> flushOutput() override
    {
        assert(m_state != State::Closed);
        assert(m_flushState != FlushState::Running);

        m_flushState = FlushState::Running;

        while (!m_outputBuffer.empty() && m_state != State::Closed)
        {
            writeData();
            co_await doIO(IOType::Write);
        }

        m_flushState = FlushState::Stopped;

        co_return;
    }

    boost::asio::awaitable<void> close() override
    {
        assert(m_state != State::Closed);

        while (m_state != State::Closed)
        {
            int ret = ssh_channel_close(m_channel);

            if (ret == SSH_AGAIN)
            {
                co_await m_socket->async_wait(boost::asio::ip::tcp::socket::wait_write, boost::asio::use_awaitable);
                continue;
            }

            assert(ret == SSH_OK);
            m_state = State::Closed;
        }

        commlog("disconnecting SSH connection");

        ssh_disconnect(m_session);
        // this has the side effect of freeing the channel
        m_channel = nullptr;
        removeConnection();

        try
        {
            m_socket->cancel();
        }
        catch (boost::system::system_error e)
        {
            commlog("error while cancelling socket IO: %s", e.what());
        }
    }

    int handleIncomingData(ssh_session session, ssh_channel channel, void* data, uint32_t len, int is_stderr)
    {
        if (m_state != State::Open)
            return 0;

        assert(session == m_session);
        assert(channel == m_channel);
        assert(is_stderr == 0);
        assert(m_waitingToRead);

        char* dataptr = reinterpret_cast<char*>(data);
        
        // TODO HACK - SSH seems to send DEL instead of backspace but expects to get backspace on the echo?
        for (size_t i = 0; i < len; i++)
        {
            if (dataptr[i] == '\x7f')
                dataptr[i] = '\b';
        }

        const size_t oldSize = m_inputBuffer.size();

        m_inputBuffer.insert(m_inputBuffer.end(), dataptr, dataptr + len);
        // TODO overflow check

        for (size_t i = oldSize; i != m_inputBuffer.size(); i++)
        {
            if (i != 0 && m_inputBuffer[i] == '\b')
            {
                m_inputBuffer.erase(m_inputBuffer.begin() + (i - 1), m_inputBuffer.begin() + (i + 1));
                i -= 2; // could cause negative underflow if i is 1, but it'll be okay when we increment
            }
        }

        // TODO we may not always be so lucky that the carriage return arrives on its own line
        if (len == 1 && *reinterpret_cast<char*>(data) == '\r')
            write("\r\n");
        else
            write(std::string_view(dataptr, len));

        return len;
    }

    void writeData()
    {
        assert(m_flushState == FlushState::Running);
        assert(m_state != State::Closed);

        size_t writableBytes = ssh_channel_window_size(m_channel);
        size_t bytesToWrite = std::min(writableBytes, m_outputBuffer.size());

        while (bytesToWrite > m_writeBuffer.size())
        {
            for (size_t i = 0; i < m_writeBuffer.size(); i++)
            {
                m_writeBuffer[i] = m_outputBuffer[i];
            }

            int ret = ssh_channel_write(m_channel, &m_writeBuffer[0], m_writeBuffer.size());
            assert(ret > 0);
            bytesToWrite -= ret;
            m_outputBuffer.erase_begin(ret);
        }

        while (bytesToWrite != 0)
        {
            for (size_t i = 0; i < bytesToWrite; i++)
            {
                m_writeBuffer[i] = m_outputBuffer[i];
            }

            int ret = ssh_channel_write(m_channel, &m_writeBuffer[0], bytesToWrite);
            assert(ret > 0);
            bytesToWrite -= ret;
            m_outputBuffer.erase_begin(ret);
        }
    }

    boost::asio::awaitable<std::string> readLine() override
    {
        size_t searchStart = 0;

        while (true)
        {
            if (!m_inputBuffer.empty())
            {
                for (auto iter = m_inputBuffer.begin(); iter != m_inputBuffer.end(); iter++)
                {
                    if (*iter == '\r') // TODO do all SSH clients only use \r?
                    {
                        std::string result{m_inputBuffer.begin(), iter};
                        m_inputBuffer.erase(m_inputBuffer.begin(), iter + 1);
                        co_return result;
                    }
                }
            }

            co_await doIO(IOType::Read);
        }
    }

  public:
    SshConnection(IOManager& ioManager, std::unique_ptr<boost::asio::ip::tcp::socket> socket, ssh_session sshSession)
        : Connection(ioManager, std::move(socket)), m_session(sshSession)
    {
        m_loop = ssh_event_new();

        // our socket is in non-blocking mode, so we have to be careful to make sure to handle SSH_AGAIN
        ssh_set_blocking(m_session, 0);

        {
            ssh_callbacks_init(&m_callbacks);

            m_callbacks.userdata = this;

            m_callbacks.auth_none_function = [](ssh_session session, const char* user, void* userdata) {
                return reinterpret_cast<SshConnection*>(userdata)->sendBanner();
            };

            m_callbacks.auth_password_function = [](ssh_session session, const char* user, const char* password,
                                                    void* userdata) {
                return reinterpret_cast<SshConnection*>(userdata)->handlePasswordAuth(user, password);
            };

            m_callbacks.auth_pubkey_function = [](ssh_session session, const char* user, ssh_key_struct* pubkey,
                                                  char signature_state, void* userdata) {
                return reinterpret_cast<SshConnection*>(userdata)->handlePubkeyAuth(user, pubkey, signature_state);
            };

            m_callbacks.channel_open_request_session_function = [](ssh_session session, void* userdata) {
                return reinterpret_cast<SshConnection*>(userdata)->openChannel();
            };
        }

        {
            ssh_callbacks_init(&m_channelCallbacks);

            m_channelCallbacks.userdata = this;

            m_channelCallbacks.channel_pty_request_function = [](ssh_session session, ssh_channel channel,
                                                                 const char* term, int x, int y, int px, int py,
                                                                 void* userdata) {
                return reinterpret_cast<SshConnection*>(userdata)->requestPty(term, x, y, px, py);
            };

            m_channelCallbacks.channel_pty_window_change_function = [](ssh_session session, ssh_channel channel,
                                                                       int width, int height, int pxwidth, int pxheight,
                                                                       void* userdata) {
                return reinterpret_cast<SshConnection*>(userdata)->updatePty(width, height, pxwidth, pxheight);
            };

            m_channelCallbacks.channel_shell_request_function = [](ssh_session session, ssh_channel channel,
                                                                   void* userdata) {
                return reinterpret_cast<SshConnection*>(userdata)->requestShell();
            };

            m_channelCallbacks.channel_data_function = [](ssh_session session, ssh_channel channel, void* data,
                                                          uint32_t len, int is_stderr, void* userdata) {
                return reinterpret_cast<SshConnection*>(userdata)->handleIncomingData(session, channel, data, len,
                                                                                      is_stderr);
            };

            m_channelCallbacks.channel_write_wontblock_function = [](ssh_session, ssh_channel, auto bytes,
                                                                     void* userdata) {
                auto connection = reinterpret_cast<SshConnection*>(userdata);
                connection->writeData();
                return 0;
            };
        }

        ssh_set_server_callbacks(m_session, &m_callbacks);

        commlog("starting SSH key exchange");
        boost::asio::co_spawn(getIOContext(), runKeyExchange(), boost::asio::detached);
    }

    // no moves or copies
    SshConnection(const SshConnection& other) = delete;
    SshConnection(SshConnection&& other) = delete;
    SshConnection& operator=(const SshConnection& other) = delete;
    SshConnection& operator=(SshConnection&& other) = delete;

    ~SshConnection()
    {
        ssh_event_free(m_loop);
        m_loop = nullptr;
        ssh_channel_free(m_channel);
        m_channel = nullptr;
        ssh_free(m_session);
        m_session = nullptr;
    }
};

IOManager::IOManager(IOManagerCallbacks callbacks, uint16_t telnetPort, uint16_t sshPort)
    : m_callbacks(callbacks), m_ioContext(), m_telnetAcceptor(m_ioContext), m_sshAcceptor(m_ioContext)
{
    // TODO ipv6?
    boost::asio::ip::tcp::endpoint telnetEndpoint(boost::asio::ip::tcp::v4(), telnetPort);
    m_telnetAcceptor.open(telnetEndpoint.protocol());
    m_telnetAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_telnetAcceptor.set_option(boost::asio::ip::tcp::acceptor::linger(true, 1000));
    m_telnetAcceptor.set_option(boost::asio::ip::tcp::no_delay(true));
    m_telnetAcceptor.bind(telnetEndpoint);
    m_telnetAcceptor.listen(-1);

    auto pendingTelnetSocket = std::make_unique<boost::asio::ip::tcp::socket>(m_ioContext);

    // some gymnastics here - pendingTelnetSocket is nullptr after the lambda is constructed because of the move
    auto pendingTelnetSocketBackup = pendingTelnetSocket.get();
    m_telnetAcceptor.async_accept(
        *pendingTelnetSocketBackup,
        [this, pendingTelnetSocket = std::move(pendingTelnetSocket)](const boost::system::error_code& error) mutable {
            this->handleNewTelnetConnection(error, std::move(pendingTelnetSocket));
        });

    boost::asio::ip::tcp::endpoint sshEndpoint(boost::asio::ip::tcp::v4(), sshPort);
    m_sshAcceptor.open(sshEndpoint.protocol());
    m_sshAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_sshAcceptor.set_option(boost::asio::ip::tcp::acceptor::linger(true, 1000));
    m_sshAcceptor.set_option(boost::asio::ip::tcp::no_delay(true));
    m_sshAcceptor.bind(sshEndpoint);
    m_sshAcceptor.listen(-1);

    m_sshBind = ssh_bind_new();
    int wideSshPort = sshPort;

    int ret = SSH_OK;

    // ret = ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_LOG_VERBOSITY_STR, "4");
    // assert(ret == SSH_OK);
    ret = ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_RSAKEY, KEYS_DIR "ssh_rsa_key");
    assert(ret == SSH_OK);
    ret = ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_BINDPORT, &wideSshPort); // probably unnecessary
    assert(ret == SSH_OK);
    ret = ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_MODULI, KEYS_DIR "moduli");
    assert(ret == SSH_OK);

    // notably, we do NOT call ssh_bind_listen because we want to own our own sockets

    auto pendingSshSocket = std::make_unique<boost::asio::ip::tcp::socket>(m_ioContext);

    auto pendingSshSocketBackup = pendingSshSocket.get();
    m_sshAcceptor.async_accept(*pendingSshSocketBackup, [this, socket = std::move(pendingSshSocket)](
                                                            const boost::system::error_code& error) mutable {
        this->handleNewSshConnection(error, std::move(socket));
    });
}

void IOManager::handleNewTelnetConnection(const boost::system::error_code& error,
                                          std::unique_ptr<boost::asio::ip::tcp::socket> socket)
{
    if (error)
    {
        auto message = error.message();
        commlog("error accepting a new telnet connection: %s", message.c_str());
    }
    else
    {
        auto connection = std::make_shared<TelnetConnection>(*this, std::move(socket));

        m_connections.push_back(connection);

        // TODO this might not return anymore?
        m_callbacks.newUnauthenticatedConnection(connection);

        auto pendingTelnetSocket = std::make_unique<boost::asio::ip::tcp::socket>(m_ioContext);
        auto pendingTelnetSocketBackup = pendingTelnetSocket.get();
        m_telnetAcceptor.async_accept(*pendingTelnetSocketBackup, [this, socket = std::move(pendingTelnetSocket)](
                                                                      const boost::system::error_code& error) mutable {
            this->handleNewTelnetConnection(error, std::move(socket));
        });
    }
}

void IOManager::handleNewSshConnection(const boost::system::error_code& error,
                                       std::unique_ptr<boost::asio::ip::tcp::socket> socket)
{
    if (error)
    {
        auto message = error.message();
        commlog("error accepting a new SSH connection: %s", message.c_str());
    }
    else
    {
        ssh_session sshSession = ssh_new(); // TODO does standard C++ have scope guards yet?

        socket->native_non_blocking(true);

        if (ssh_bind_accept_fd(m_sshBind, sshSession, socket->native_handle()) == SSH_ERROR)
        {
            commlog("error accepting SSH connection from fd: ", ssh_get_error(m_sshBind));
            ssh_free(sshSession);
            return;
        }

        auto connection = std::make_shared<SshConnection>(*this, std::move(socket), sshSession);

        m_connections.push_back(connection);

        auto pendingSshSocket = std::make_unique<boost::asio::ip::tcp::socket>(m_ioContext);
        auto pendingSshSocketBackup = pendingSshSocket.get();
        m_sshAcceptor.async_accept(*pendingSshSocketBackup, [this, socket = std::move(pendingSshSocket)](
                                                                const boost::system::error_code& error) mutable {
            this->handleNewSshConnection(error, std::move(socket));
        });
    }
}

void IOManager::notifyGameUnauthenticatedUserConnected(Connection& connection)
{
    std::shared_ptr<Connection> sharedConn;

    for (auto& listConn : m_connections)
    {
        if (listConn.get() == &connection)
        {
            sharedConn = listConn;
            break;
        }
    }

    assert(sharedConn.get() != nullptr);
    m_callbacks.newUnauthenticatedConnection(sharedConn);
}

void IOManager::notifyGameAuthenticatedUserConnected(Connection& connection, const std::string& user)
{
    std::shared_ptr<Connection> sharedConn;

    for (auto& listConn : m_connections)
    {
        if (listConn.get() == &connection)
        {
            sharedConn = listConn;
            break;
        }
    }

    assert(sharedConn.get() != nullptr);
    m_callbacks.newAuthenticatedConnection(sharedConn, user);
}

void IOManager::removeConnection(Connection* connection)
{
    auto iter = std::find_if(m_connections.begin(), m_connections.end(),
                             [connection](std::shared_ptr<Connection> conn) { return conn.get() == connection; });

    assert(iter != m_connections.end());

    // the game will still be holding a reference
    assert((*iter).use_count() == 2);

    m_connections.erase(iter);
}

std::string IOManager::getBanner()
{
    return m_callbacks.getWelcomeBanner();
}

void IOManager::runUntil(std::chrono::steady_clock::time_point time)
{
    m_ioContext.run_until(time);
}

boost::asio::awaitable<void> IOManager::waitForPulse(std::chrono::steady_clock::time_point pulseTime)
{
    boost::asio::steady_timer timer(m_ioContext, pulseTime);

    co_await timer.async_wait(boost::asio::use_awaitable);

    co_return;
}

void Connection::write(std::string_view data)
{
    if (m_state != State::Open)
    {
        commlog("write called on closing socket");
        return;
    }

    if (m_outputBuffer.capacity() - m_outputBuffer.size() < data.size())
    {
        commlog("output buffer overflow");
        close();
        // notifyGameConnectionClosed();
        return;
    }

    m_outputBuffer.insert(m_outputBuffer.end(), data.begin(), data.end());

    if (m_flushState == FlushState::Stopped)
    {
        m_flushState = FlushState::Starting;

        // TODO this should get wrapped in a call to boost::asio::defer so it happens after whatever call is triggering the write
        // ... but that has some lifecycle issues I haven't sorted out yet
        boost::asio::co_spawn(this->getIOContext(), this->flushOutput(), boost::asio::detached);
    }
}

boost::asio::awaitable<void> Connection::flushAndClose()
{
    co_await flushOutput();
    co_await close();
}

const std::string& Connection::getHostname() const
{
    return m_hostname;
}

const std::string& Connection::getIpAddress() const
{
    return m_address;
}

int Connection::getPort() const
{
    return m_socket->remote_endpoint().port();
}
