#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <cstdint>
#include <boost/asio.hpp>
#include <boost/circular_buffer_fwd.hpp>
#include <chrono>
#include <optional>
#include <array>
#include <libssh/server.h>

#include "mud.hxx"

// TODO how can new player creation over SSH work? Username "new" and password any?

struct Pubkey
{
    std::string type;
    std::string key;
};

typedef std::shared_ptr<void> ConnectionContext;

struct IOManagerCallbacks
{
    // Notification of a new command from a connection. Only delivered once per call to IOManager::runUntil
    void (*commandReceived)(ConnectionContext context, const std::string& line);
    // Notification of a window size change
    void (*windowChangedSize)(ConnectionContext context, int width, int height);
    // Notification of a new unauthenticated connection, must return the desired context pointer for future calls.
    ConnectionContext (*newUnauthenticatedConnection)(std::shared_ptr<Connection> connection);
    // Request to check if the username/password match a known user
    bool (*authenticateUser)(const std::string& username, const std::string& password);
    // Request to retrieve the public keys of a known user
    std::vector<Pubkey> (*getPublicKeysForUser)(const std::string& username);
    // Notification of a new authenticated connection, with authenticated username. Must return the desired context
    // pointer for future calls.
    ConnectionContext (*newAuthenticatedConnection)(std::shared_ptr<Connection> connection,
                                                                           const std::string& username);
    void (*connectionClosed)(ConnectionContext context);
};

class TelnetConnection;
class SshConnection;

class IOManager
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
    void notifyGameWindowSizeChanged(ConnectionContext context, int width, int height);
    void sendCommandToGame(ConnectionContext context, const std::string& command);
    void notifyGameConnectionClosed(ConnectionContext context);
    void removeConnection(Connection* connection);

  public:
    IOManager(IOManagerCallbacks callbacks, uint16_t telnetPort, uint16_t sshPort);
    void runUntil(std::chrono::steady_clock::time_point time);
};

class Connection
{
  private:
    bool m_buffered = true;

  protected:
    enum class State
    {
        Open,
        Flushing,
        Closed
    };

    State m_state = State::Open;
    ConnectionContext m_context = nullptr;
    IOManager& m_manager;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;
    boost::circular_buffer<char> m_inputBuffer;
    boost::circular_buffer<char> m_outputBuffer;

    std::string m_hostname;
    std::string m_address;
    bool m_commandThisPulse = false;
    bool m_writerRunning = false;
    bool m_readerRunning = false;

    friend class IOManager;
    void setContext(ConnectionContext context);

    void scanAndSendCommand(bool newPulse = false);

    virtual void startWriting() = 0;
    virtual void startClosing() = 0;
    virtual void startFlushingAndClose() = 0;

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

    void notifyGameWindowSizeChanged(int width, int height)
    {
        m_manager.notifyGameWindowSizeChanged(m_context, width, height);
    }

    void notifyGameConnectionClosed()
    {
        m_manager.notifyGameConnectionClosed(m_context);
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
    // close the connection immediately
    void close();
    // try to flush buffered data and then close the connection
    void flushAndClose();
    void setLineBuffered(bool buffered);

    const std::string& getHostname() const;
    const std::string& getIpAddress() const;
    int getPort() const;
};