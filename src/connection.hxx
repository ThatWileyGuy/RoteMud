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

struct IOManagerCallbacks
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
    void removeConnection(Connection* connection);

  public:
    IOManager(IOManagerCallbacks callbacks, uint16_t telnetPort, uint16_t sshPort);
    void runUntil(std::chrono::steady_clock::time_point time);
};

class Connection
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