#ifdef WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <SDKDDKVer.h>
#endif

#include "connection.hxx"

#include <boost/circular_buffer.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <libssh/callbacks.h>
#include <iostream>

#define commlog(str, ...) log(str, LOG_COMM, sysdata.log_level, ##__VA_ARGS__)

class TelnetConnection : public Connection
{
  private:
    std::array<char, MAX_INPUT_LENGTH> m_readBuffer;
    std::array<char, MAX_OUTPUT_SIZE> m_writeBuffer;

    void startWriting() override
    {
        assert(m_state != State::Closed);
        assert(!m_outputBuffer.empty());
        assert(m_writerRunning == false);

        const size_t bytesToSend = std::min(m_writeBuffer.size(), m_outputBuffer.size());

        for (size_t i = 0; i < bytesToSend; i++)
        {
            m_writeBuffer[i] = m_outputBuffer[i];
        }

        m_outputBuffer.erase_begin(bytesToSend);

        m_writerRunning = true;

        m_socket->async_write_some(
            boost::asio::buffer(&m_writeBuffer[0], m_writeBuffer.size()),
            [this, size = m_writeBuffer.size()](const boost::system::error_code& error, size_t transferred) {
                this->finishWriting(error, size, transferred);
            });
    }

    void finishWriting(const boost::system::error_code& error, size_t attempted, size_t transferred)
    {
        assert(m_writerRunning == true);

        m_writerRunning = false;

        // bail if the socket closed but keep writing if we're flushing
        if (m_state == State::Closed)
        {
            finishClosing();
            return;
        }

        if (error)
        {
            std::string message = error.message();
            startClosing();
            notifyGameConnectionClosed();
            commlog("write failure: %s", message.c_str());
            return;
        }

        if (transferred < attempted)
        {
            const size_t stillToWrite = attempted - transferred;
            std::memmove(&m_writeBuffer[0], &m_writeBuffer[transferred], stillToWrite);

            m_writerRunning = true;

            m_socket->async_write_some(
                boost::asio::buffer(&m_writeBuffer[0], stillToWrite),
                [this, stillToWrite](const boost::system::error_code& error, size_t transferred) {
                    this->finishWriting(error, stillToWrite, transferred);
                });

            return;
        }

        if (!m_outputBuffer.empty())
        {
            startWriting();
        }
        else if (m_state == State::Flushing)
        {
            // we're done flushing - time to close down
            startClosing();
            finishClosing();
        }
    }

    void startReading()
    {
        assert(m_state == State::Open);
        assert(m_readerRunning == false);

        m_readerRunning = true;

        m_socket->async_read_some(boost::asio::buffer(&m_readBuffer[0], m_readBuffer.size()),
                                  [this](const boost::system::error_code& error, size_t transferred) {
                                      this->finishReading(error, transferred);
                                  });
    }

    void finishReading(const boost::system::error_code& error, size_t transferred)
    {
        assert(m_readerRunning);

        m_readerRunning = false;

        // if the socket is going away, go ahead and just stop
        if (m_state != State::Open)
        {
            finishClosing();
            return;
        }

        if (error)
        {
            std::string message = error.message();
            commlog("read failure: %s", message.c_str());
            startClosing(); // if the socket has an error, flushing it probably won't work anyway
            notifyGameConnectionClosed();
            return;
        }

        if (transferred > (m_inputBuffer.capacity() - m_inputBuffer.size()))
        {
            commlog("read buffer overflow");
            write("Too much input. Bye!\r\n");
            flushAndClose();
            notifyGameConnectionClosed();
            return;
        }

        m_inputBuffer.insert(m_inputBuffer.end(), &m_readBuffer[0], &m_readBuffer[transferred]);

        startReading();

        scanAndSendCommand();
    }

    void finishClosing()
    {
        assert(!m_writerRunning);
        assert(!m_readerRunning);
        assert(m_context == nullptr);

        if (m_state == State::Flushing)
        {
            startClosing();
        }

        removeConnection();
    }

  public:
    TelnetConnection(IOManager& ioManager, std::unique_ptr<boost::asio::ip::tcp::socket> socket)
        : Connection(ioManager, std::move(socket))
    {
        // nothing fancy for telnet - go straight into reading
        startReading();
    }

    void startClosing() override
    {
        assert(m_state != State::Closed);
        m_state = State::Closed;
        m_context = nullptr;
        m_socket->close();
    }

    void startFlushingAndClose() override
    {
        assert(m_state == State::Open);
        m_state = State::Flushing;
        m_context = nullptr;
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
    size_t m_writableBytes = 0;
    std::array<char, MAX_OUTPUT_SIZE> m_writeBuffer;
    std::string m_authenticatedUsername;

    void continueKeyExchange(const boost::system::error_code& error)
    {
        assert(m_readerRunning && !m_writerRunning);

        m_readerRunning = false;

        if (error)
        {
            std::string message = error.message();
            commlog("SSH connection error during key exchange: %s", message.c_str());
            startClosing();
            return;
        }

        int ret = ssh_handle_key_exchange(m_session);

        if (ret == SSH_ERROR)
        {
            commlog("SSH key exchange failed: %s", ssh_get_error(m_session));
            commlog("SSH error code: %d", ssh_get_error_code(m_session));
            startClosing();
            return;
        }
        else if (ret == SSH_AGAIN)
        {
            commlog("continuing SSH key exchange...");

            m_readerRunning = true;

            m_socket->async_wait(boost::asio::ip::tcp::socket::wait_read,
                                 boost::bind(&SshConnection::continueKeyExchange, this, _1));
            return;
        }
        else
        {
            assert(ret == SSH_OK);

            commlog("completed key exchange");

            ssh_set_auth_methods(m_session, SSH_AUTH_METHOD_PASSWORD | SSH_AUTH_METHOD_PUBLICKEY);
            ssh_event_add_session(m_loop, m_session);

            m_readerRunning = true;
            normalLoop(boost::system::error_code(), LoopType::Read);
            m_writerRunning = true;
            normalLoop(boost::system::error_code(), LoopType::Write);
            return;
        }
    }

    enum class LoopType
    {
        Read,
        Write
    };

    void normalLoop(const boost::system::error_code& error, LoopType loopType)
    {
        bool& loopRunning = (loopType == LoopType::Read) ? m_readerRunning : m_writerRunning;

        // This is a bit nasty - because this function runs when Boost thinks the socket is readable or writable
        // but we can't tell libssh whether it should be reading or writing, we may have finished some work
        // when we were not supposed to be doing it.
        if (!loopRunning)
            return;

        if (m_state == State::Closed || (loopType == LoopType::Read && m_state == State::Flushing))
        {
            loopRunning = false;
            finishClosing();
            return;
        }

        if (error)
        {
            std::string message = error.message();
            commlog("SSH socket error: %s", message.c_str());
            loopRunning = false;
            startClosing();
            return;
        }

        int ret = ssh_event_dopoll(m_loop, 0);

        if (ret == SSH_ERROR)
        {
            commlog("SSH connection error: %s", ssh_get_error(m_session));
            commlog("SSH error code: %d", ssh_get_error_code(m_session));
            loopRunning = false;
            startClosing();
            return;
        }

        if (m_state == State::Closed)
        {
            loopRunning = false;
            finishClosing();
            return;
        }

        if (loopRunning)
        {
            auto waitType = (loopType == LoopType::Read) ? boost::asio::ip::tcp::socket::wait_read
                                                         : boost::asio::ip::tcp::socket::wait_write;

            m_socket->async_wait(waitType, boost::bind(&SshConnection::normalLoop, this, _1, loopType));
        }
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
        if (signatureState == SSH_PUBLICKEY_STATE_NONE)
            return SSH_AUTH_SUCCESS;
        if (signatureState != SSH_PUBLICKEY_STATE_VALID)
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
        return 0;
    }

    int updatePty(int x, int y, int px, int py)
    {
        commlog("PTY updated: %d %d %d %d", x, y, px, py);
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

        notifyGameAuthenticatedUserConnected(m_authenticatedUsername);
        return 0;
    }

    void startWriting()
    {
        assert(m_state != State::Closed);
        assert(!m_outputBuffer.empty() || m_state == State::Flushing);
        assert(!m_writerRunning);

        m_writerRunning = true;

        writeData();

        // Always get at least one callback so we update m_writableBytes for next time
        assert(!m_writerRunning);
        m_writerRunning = true;
        m_socket->async_wait(boost::asio::ip::tcp::socket::wait_write,
                             boost::bind(&SshConnection::normalLoop, this, _1, LoopType::Write));
    }

    int handleIncomingData(ssh_session session, ssh_channel channel, void* data, uint32_t len, int is_stderr)
    {
        if (m_state != State::Open)
            return 0;

        assert(session == m_session);
        assert(channel == m_channel);
        assert(is_stderr == 0);
        assert(m_readerRunning);

        char* dataptr = reinterpret_cast<char*>(data);

        m_inputBuffer.insert(m_inputBuffer.end(), dataptr, dataptr + len);
        // TODO overflow check

        // TODO we may not always be so lucky that the carriage return arrives on its own line
        if (len == 1 && *reinterpret_cast<char*>(data) == '\r')
            write("\r\n");
        else
            write(std::string_view(reinterpret_cast<char*>(data), len));

        scanAndSendCommand();

        return len;
    }

    void writeData()
    {
        assert(m_writerRunning);
        assert(m_state != State::Closed);

        // some gymnastics here - it turns out we only get this notification on the poll call after a write, so we have
        // to store how many bytes we can write for next time
        size_t bytesToWrite = std::min(m_writableBytes, m_outputBuffer.size());
        const size_t leftoverWritableBytes = m_writableBytes - bytesToWrite;

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

        m_writableBytes = leftoverWritableBytes;

        if (m_outputBuffer.empty())
        {
            if (m_state == State::Flushing)
            {
                int ret = ssh_channel_close(m_channel);

                if (ret == SSH_AGAIN)
                {
                    assert(m_writerRunning);
                }
                else
                {
                    assert(ret == SSH_OK);
                    startClosing();
                }
            }
            else
            {
                m_writerRunning = false;
            }
        }
    }

    void startClosing() override
    {
        assert(m_state != State::Closed);
        m_state = State::Closed;
        m_context = nullptr;
        try
        {
            m_socket->cancel();
        }
        catch (boost::system::system_error e)
        {
            commlog("error while cancelling socket IO: %s", e.what());
        }

        finishClosing();
    }

    void startFlushingAndClose() override
    {
        assert(m_state == State::Open);
        m_state = State::Flushing;
        m_context = nullptr;

        if (!m_writerRunning)
        {
            startWriting();
        }
    }

    void finishClosing()
    {
        if (!m_writerRunning && !m_readerRunning)
        {
            std::cout << "SSH connection closing" << std::endl;
            ssh_disconnect(m_session);
            // this has the side effect of freeing the channel
            m_channel = nullptr;
            assert(m_context == nullptr);
            removeConnection();
        }
        else
        {
            std::cout << "SSH connection not ready to close: " << m_writerRunning << " " << m_readerRunning
                      << std::endl;
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

            m_channelCallbacks.channel_write_wontblock_function = [](ssh_session, ssh_channel, size_t bytes,
                                                                     void* userdata) {
                auto connection = reinterpret_cast<SshConnection*>(userdata);
                connection->m_writableBytes = bytes;
                reinterpret_cast<SshConnection*>(userdata)->writeData();
                return 0;
            };
        }

        ssh_set_server_callbacks(m_session, &m_callbacks);

        commlog("starting SSH key exchange");
        m_readerRunning = true;
        continueKeyExchange(boost::system::error_code());
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

    int ret = ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_DSAKEY, KEYS_DIR "ssh_dsa_key");
    assert(ret == SSH_OK);
    ret = ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_RSAKEY, KEYS_DIR "ssh_rsa_key");
    assert(ret == SSH_OK);
    ret = ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_BINDPORT, &wideSshPort); // probably unnecessary
    assert(ret == SSH_OK);
    ret = ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_BANNER, "This is a custom banner??????");
    assert(ret == SSH_OK);
    // ret = ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_LOG_VERBOSITY_STR, "4");
    // assert(ret == SSH_OK);
#ifdef WIN32
    ssh_bind_options_set(m_sshBind, SSH_BIND_OPTIONS_MODULI, KEYS_DIR "moduli");
    assert(ret == SSH_OK);
#endif

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

        auto context = m_callbacks.newUnauthenticatedConnection(connection);
        connection->setContext(context);

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
    sharedConn->setContext(m_callbacks.newUnauthenticatedConnection(sharedConn));
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
    sharedConn->setContext(m_callbacks.newAuthenticatedConnection(sharedConn, user));
}

void IOManager::sendCommandToGame(void* context, const std::string& command)
{
    if (context == nullptr)
        bug("command from null context");
    else
        m_callbacks.commandReceived(context, command);
}

void IOManager::notifyGameConnectionClosed(void* context)
{
    if (context != nullptr)
        m_callbacks.connectionClosed(context);
}

void IOManager::removeConnection(Connection* connection)
{
    auto iter = std::find_if(m_connections.begin(), m_connections.end(),
                             [connection](std::shared_ptr<Connection> conn) { return conn.get() == connection; });

    assert(iter != m_connections.end());

    assert((*iter).use_count() == 1);
    assert(iter->get()->m_context == nullptr);

    m_connections.erase(iter);
}

void IOManager::runUntil(std::chrono::steady_clock::time_point time)
{
    for (auto& connection : m_connections)
    {
        connection->scanAndSendCommand(true);
    }

    m_ioContext.run_until(time);
}

void Connection::setContext(void* context)
{
    m_context = context;
}

void Connection::scanAndSendCommand(bool newPulse)
{
    if (newPulse)
    {
        m_commandThisPulse = false;
    }
    else if (m_commandThisPulse)
    {
        return;
    }

    if (m_inputBuffer.empty())
        return;

    size_t lineEnd = 0;
    for (lineEnd = 0; lineEnd < m_inputBuffer.size(); lineEnd++)
    {
        char c = m_inputBuffer[lineEnd];
        if (c == '\n' || c == '\r')
            break;

        assert(c != '\0');
    }

    if (lineEnd == m_inputBuffer.size())
    {
        // no line ending, so no complete command
        return;
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

    // now dispatch
    m_manager.sendCommandToGame(m_context, result);
    m_commandThisPulse = true;
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
        notifyGameConnectionClosed();
        return;
    }

    m_outputBuffer.insert(m_outputBuffer.end(), data.begin(), data.end());

    if (!m_writerRunning)
    {
        startWriting();
    }
}

void Connection::close()
{
    startClosing();
}

void Connection::flushAndClose()
{
    startFlushingAndClose();
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
