/**
 * @file TCPServer.h
 * @brief Declaration of the TCPServer class template for managing a multithreaded TCP server.
 */

#ifndef interprocess_tcp_server_h
#define interprocess_tcp_server_h

#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/system/error_code.hpp>

namespace interprocess {

/**
 * @class TCPServer
 * @brief A class template for managing a multithreaded TCP server.
 *
 * @tparam ConnHandlerT The connection handler type.
 * @tparam AcceptorT The acceptor type (default: boost::asio::ip::tcp::acceptor).
 */
template<typename ConnHandlerT, typename AcceptorT = boost::asio::ip::tcp::acceptor>
class TCPServer 
{
    using shared_conn_handler_t = std::shared_ptr<ConnHandlerT>;

public:
    /**
     * @brief Constructor for the TCPServer class.
     * @param context Reference to the Boost.Asio I/O context.
     * @param numThreads The number of threads to use for the server (default: 1).
     */
    TCPServer(boost::asio::io_context& context, const size_t numThreads = 1);

    /**
     * @brief Destructor for the TCPServer class.
     */
    virtual ~TCPServer();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    TCPServer(const TCPServer&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     * @return Reference to the updated instance (not used).
     */
    TCPServer& operator=(const TCPServer&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving.
     */
    TCPServer(TCPServer&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving.
     * @return Reference to the updated instance (not used).
     */
    TCPServer& operator=(TCPServer&&) = delete;

    /**
     * @brief Start the TCP server and begin accepting connections.
     * @param endPoint The endpoint to bind the server to.
     */
    void Start(const boost::asio::ip::tcp::endpoint& endPoint);

    /**
     * @brief Join all threads in the server's thread pool, blocking until they complete.
     */
    void Join();

    /**
     * @brief Get the number of threads in the server's thread pool.
     * @return The number of threads.
     */
    size_t GetNumThreads() const;

    /**
     * @brief Get the acceptor used by the server to accept new connections.
     * @return Reference to the server's acceptor.
     */
    AcceptorT& GetAcceptor();

private:
    /**
     * @brief Handle a new incoming connection.
     * @param handler Shared pointer to the connection handler.
     * @param ec The error code, if any, for the connection attempt.
     */
    void HandleNewConnection(shared_conn_handler_t handler, const boost::system::error_code ec);

    /** @brief The number of threads used by the server. */
    const size_t numThreads_;

    /** @brief The thread pool for handling server operations. */
    std::vector<std::thread> threadPool_;

    /** @brief Reference to the Boost.Asio I/O context. */
    boost::asio::io_context& ioService_;

    /** @brief The acceptor used for accepting new connections. */
    AcceptorT acceptor_;
};

#include "TCPServer.hpp"

} // end namespace interprocess

#endif // interprocess_tcp_server_h