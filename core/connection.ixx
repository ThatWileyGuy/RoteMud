module;

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef WIN32
#include <SDKDDKVer.h>
#endif

#include <cstdint>
#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/bind/bind.hpp>
#include <chrono>
#include <optional>
#include <array>
#include <libssh/server.h>
#include <libssh/callbacks.h>

#include "mud.hxx"

export module connection;

// TODO how can new player creation over SSH work? Username "new" and password any?

export struct Pubkey
{
    std::string type;
    std::string key;
};

export struct IOManagerCallbacks
{
    // Notification of a new unauthenticated connection, must return the desired context pointer for future calls.
    void (*newUnauthenticatedConnection)(std::shared_ptr<Connection> connection);
    // Request to check if the username/password match a known user
    bool (*authenticateUser)(const std::string& username, const std::string& password);
    // Request to retrieve the public keys of a known user
    std::vector<Pubkey> (*getPublicKeysForUser)(const std::string& username);
    // Notification of a new authenticated connection, with authenticated username. Must return the desired context
    // pointer for future calls.
    void (*newAuthenticatedConnection)(std::shared_ptr<Connection> connection, const std::string& username);
};

class TelnetConnection;
class SshConnection;

export class IOManager
{
  private:
    IOManagerCallbacks m_callbacks;
    boost::asio::io_context m_ioContext;
    boost::asio::ip::tcp::acceptor m_telnetAcceptor;
    boost::asio::ip::tcp::acceptor m_sshAcceptor;
    ssh_bind m_sshBind;

    std::vector<std::shared_ptr<Connection>> m_connections;

    void handleNewTelnetConnection(const boost::system::error_code& error,
                                   std::unique_ptr<boost::asio::ip::tcp::socket> socket);
    void handleNewSshConnection(const boost::system::error_code& error,
                                std::unique_ptr<boost::asio::ip::tcp::socket> socket);

  protected:
    friend class Connection;
    friend class SshConnection;
    void notifyGameUnauthenticatedUserConnected(Connection& connection);
    void notifyGameAuthenticatedUserConnected(Connection& connection, const std::string& user);
    void removeConnection(Connection* connection);

  public:
    IOManager(IOManagerCallbacks callbacks, uint16_t telnetPort, uint16_t sshPort);
    void runUntil(std::chrono::steady_clock::time_point time);
};

export class Connection
{
  private:
  protected:
    enum class State
    {
        Open,
        Flushing,
        Closed
    };

    State m_state = State::Open;
    IOManager& m_manager;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;
    boost::circular_buffer<char> m_inputBuffer;
    boost::circular_buffer<char> m_outputBuffer;

    std::string m_hostname;
    std::string m_address;
    bool m_waitingForIO = false;

    friend class IOManager;

    Connection(IOManager& ioManager, std::unique_ptr<boost::asio::ip::tcp::socket> socket)
        : m_manager(ioManager), m_socket(std::move(socket)), m_inputBuffer(MAX_INBUF_SIZE),
          m_outputBuffer(64 * MAX_INBUF_SIZE) // TODO this should be a constant
    {
        m_address = m_socket->remote_endpoint().address().to_string();
        boost::asio::io_service ioService;
        boost::asio::ip::tcp::resolver resolver(ioService);
        boost::asio::ip::tcp::resolver::iterator destination = resolver.resolve(m_socket->remote_endpoint());
        m_hostname = destination->host_name();
    }

    virtual ~Connection()
    {
    }

    void notifyGameAuthenticatedUserConnected(const std::string& user)
    {
        m_manager.notifyGameAuthenticatedUserConnected(*this, user);
    }

    void removeConnection()
    {
        m_manager.removeConnection(this);
    }

    bool requestPasswordAuth(const std::string& username, const std::string& password)
    {
        return m_manager.m_callbacks.authenticateUser(username, password);
    }

    std::vector<Pubkey> getPubkeysForUser(const std::string& username)
    {
        return m_manager.m_callbacks.getPublicKeysForUser(username);
    }

  public:
    // send data to the client (via a buffer)
    void write(std::string_view data);
    // try to flush buffered data and then close the connection
    boost::asio::awaitable<void> flushAndClose();

    const std::string& getHostname() const;
    const std::string& getIpAddress() const;
    int getPort() const;

    virtual boost::asio::awaitable<std::string> readLine() = 0;
    virtual boost::asio::awaitable<void> flushOutput() = 0;
    virtual boost::asio::awaitable<void> close() = 0;

    boost::asio::io_context& getIOContext() const
    {
        return m_manager.m_ioContext;
    }
};

#define commlog(str, ...) log(str, LOG_COMM, sysdata.log_level, ##__VA_ARGS__)

class TelnetConnection : public Connection
{
  private:
    boost::asio::awaitable<void> readSome()
    {
        std::array<char, MAX_INPUT_LENGTH> readBuffer;
        assert(m_state == State::Open);
        assert(m_waitingForIO == false);

        m_waitingForIO = true;

        size_t bytesRead = co_await m_socket->async_read_some(boost::asio::buffer(&readBuffer[0], readBuffer.size()),
                                                              boost::asio::use_awaitable);

        // TODO error handling
        // TODO what happens if we overflow m_inputBuffer
        m_inputBuffer.insert(m_inputBuffer.end(), &readBuffer[0], &readBuffer[bytesRead]);

        m_waitingForIO = false;
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
        assert(m_waitingForIO == false);

        m_waitingForIO = true;

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

        m_waitingForIO = false;
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
    size_t m_writableBytes = 0;
    std::array<char, MAX_OUTPUT_SIZE> m_writeBuffer;
    std::string m_authenticatedUsername;
    bool m_shellRequested = false;

    boost::asio::awaitable<void> runKeyExchange()
    {
        assert(!m_waitingForIO);

        m_waitingForIO = true;

        while (true)
        {
            co_await m_socket->async_wait(boost::asio::ip::tcp::socket::wait_read, boost::asio::use_awaitable);

            // TODO error handling
            /*
            if (error)
            {
                std::string message = error.message();
                commlog("SSH connection error during key exchange: %s", message.c_str());
                startClosing();
                return;
            }
            */

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
                ssh_event_add_session(m_loop, m_session);

                m_waitingForIO = false;

                while (!m_shellRequested)
                {
                    co_await doIO(IOType::Read);
                }

                notifyGameAuthenticatedUserConnected(m_authenticatedUsername);
                co_return;
            }
            else
            {
                assert(ret = SSH_ERROR);
                commlog("SSH key exchange failed: %s", ssh_get_error(m_session));
                commlog("SSH error code: %d", ssh_get_error_code(m_session));
                close();
                co_return;
            }
        }

        m_waitingForIO = false;
    }

    enum class IOType
    {
        Read,
        Write
    };

    boost::asio::awaitable<void> doIO(IOType loopType)
    {
        assert(!m_waitingForIO);
        m_waitingForIO = true;

        const auto waitType = (loopType == IOType::Read) ? boost::asio::ip::tcp::socket::wait_read
                                                         : boost::asio::ip::tcp::socket::wait_write;

        co_await m_socket->async_wait(waitType, boost::asio::use_awaitable);

        int ret = ssh_event_dopoll(m_loop, 0);

        m_waitingForIO = false;

        if (ret == SSH_ERROR)
        {
            commlog("SSH connection error: %s", ssh_get_error(m_session));
            commlog("SSH error code: %d", ssh_get_error_code(m_session));
            m_waitingForIO = false;
            // TODO throw exception that the socket needs to close
            co_return;
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

        while (!m_outputBuffer.empty() && m_state != State::Closed)
        {
            co_await doIO(IOType::Write);
        }

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
        assert(m_waitingForIO);

        char* dataptr = reinterpret_cast<char*>(data);

        for (char* printptr = dataptr; printptr != (dataptr + len); printptr++)
        {
            if (isprint(*printptr))
                commlog("got %c", *printptr);
            else
                commlog("got 0x%x", (unsigned int)*printptr);
        }

        m_inputBuffer.insert(m_inputBuffer.end(), dataptr, dataptr + len);
        // TODO overflow check

        // TODO we may not always be so lucky that the carriage return arrives on its own line
        if (len == 1 && *reinterpret_cast<char*>(data) == '\r')
            write("\r\n");
        else
            write(std::string_view(reinterpret_cast<char*>(data), len));

        return len;
    }

    void writeData()
    {
        assert(m_waitingForIO);
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
    }

    boost::asio::awaitable<std::string> readLine() override
    {
        size_t searchStart = 0;

        while (true)
        {
            if (!m_inputBuffer.empty())
            {
                for (searchStart; searchStart < m_inputBuffer.size(); searchStart++)
                {
                    if (m_inputBuffer[searchStart] == '\r') // TODO do all SSH clients only use \r?
                    {
                        std::string result{m_inputBuffer.begin(), m_inputBuffer.begin() + searchStart};
                        m_inputBuffer.erase_begin(searchStart + 1);
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
                connection->m_writableBytes = bytes;
                reinterpret_cast<SshConnection*>(userdata)->writeData();
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

    assert((*iter).use_count() == 1);

    m_connections.erase(iter);
}

void IOManager::runUntil(std::chrono::steady_clock::time_point time)
{
    m_ioContext.run_until(time);
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
