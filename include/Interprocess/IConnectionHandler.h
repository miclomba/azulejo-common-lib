/**
 * @file IConnectionHandler.h
 * @brief Declaration of the IConnectionHandler interface for managing network connections.
 */

#ifndef interprocess_iconnection_handler_h
#define interprocess_iconnection_handler_h

#include <deque>
#include <memory>
#include <mutex>
#include <vector>

#include "Interprocess/config.h"

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

#include "Interprocess/AsioAdapter.h"

namespace interprocess
{

    /**
     * @class IConnectionHandler
     * @brief An interface for managing TCP network connections using Boost.Asio.
     *
     * @tparam PODType The Plain Old Data (POD) type used for messages.
     * @tparam AsioAdapterT The adapter type for integrating with Boost.Asio (default: AsioAdapter<PODType>).
     */
    template <typename PODType, typename AsioAdapterT = AsioAdapter<PODType>>
    class IConnectionHandler : public std::enable_shared_from_this<IConnectionHandler<PODType, AsioAdapterT>>
    {
    public:
        /**
         * @brief Alias for a shared pointer to the IConnectionHandler.
         */
        using shared_conn_handler_t = std::shared_ptr<IConnectionHandler>;

        /**
         * @brief Constructor for the IConnectionHandler class.
         * @param ioService Reference to the Boost.Asio I/O context.
         */
        IConnectionHandler(boost::asio::io_context &ioService);

        /**
         * @brief Virtual destructor for the IConnectionHandler class.
         */
        virtual ~IConnectionHandler();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        IConnectionHandler(const IConnectionHandler &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        IConnectionHandler &operator=(const IConnectionHandler &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        IConnectionHandler(IConnectionHandler &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        IConnectionHandler &operator=(IConnectionHandler &&) = delete;

        /**
         * @brief Start the application logic for the connection handler.
         * @param thisHandler Shared pointer to the current connection handler instance.
         */
        virtual void StartApplication(shared_conn_handler_t thisHandler) = 0;

        /**
         * @brief Post a request to receive messages.
         */
        void PostReceiveMessages();

        /**
         * @brief Check if there are received messages in the queue.
         * @return True if there are received messages, false otherwise.
         */
        bool HasReceivedMessages() const;

        /**
         * @brief Retrieve one received message from the queue.
         * @return A vector containing the received message data.
         */
        std::vector<PODType> GetOneMessage();

        /**
         * @brief Post a message to be sent to the outgoing message queue.
         * @param message The message data to send.
         */
        void PostOutgoingMessage(std::vector<PODType> message);

        /**
         * @brief Check if there are outgoing messages in the queue.
         * @return True if there are outgoing messages, false otherwise.
         */
        bool HasOutgoingMessages() const;

        /**
         * @brief Connect to a remote endpoint.
         * @param endPoints The list of resolved endpoints to connect to.
         */
        void Connect(boost::asio::ip::tcp::resolver::results_type endPoints);

        /**
         * @brief Close the connection.
         */
        void Close();

        /**
         * @brief Get the TCP socket associated with the connection handler.
         * @return Reference to the TCP socket.
         */
        boost::asio::ip::tcp::socket &Socket();

        /**
         * @brief Get the Boost.Asio I/O context associated with the connection handler.
         * @return Reference to the I/O context.
         */
        boost::asio::io_context &IOService();

        /**
         * @brief Get the I/O adapter associated with the connection handler.
         * @return Reference to the I/O adapter.
         */
        AsioAdapterT &IOAdapter();

    private:
        /**
         * @brief Queue a received message after data transfer.
         * @param bytesTransferred The number of bytes received.
         */
        void QueueReceivedMessage(size_t bytesTransferred);

        /**
         * @brief Get the length of the received data.
         * @param bytesTransferred The number of bytes received.
         * @return The length of the received data.
         */
        size_t ReceivedDataLength(const size_t bytesTransferred);

        /**
         * @brief Wrap a message with a header for outgoing transmission.
         * @param message The message data to wrap.
         * @return A vector containing the wrapped message.
         */
        std::vector<PODType> WrapWithHeader(const std::vector<PODType> message) const;

        /**
         * @brief Start sending a message from the outgoing queue.
         */
        void SendMessageStart();

        /**
         * @brief Complete the sending of the current message.
         */
        void SendMessageDone();

        /** @brief Length of the header used in messages. */
        const size_t HEADER_LENGTH = 4;

        /** @brief Buffer for incoming message data. */
        std::vector<char> inMessage_;

        /** @brief Mutex for synchronizing access to the read queue. */
        mutable std::mutex readLock_;

        /** @brief Strand for managing read operations. */
        boost::asio::io_context::strand readStrand_;

        /** @brief Queue of received messages. */
        std::deque<std::vector<PODType>> inMessageQue_;

        /** @brief Mutex for synchronizing access to the write queue. */
        mutable std::mutex writeLock_;

        /** @brief Strand for managing write operations. */
        boost::asio::io_context::strand writeStrand_;

        /** @brief Queue of outgoing messages. */
        std::deque<std::vector<PODType>> outMessageQue_;

        /** @brief Reference to the Boost.Asio I/O context. */
        boost::asio::io_context &ioServiceRef_;

        /** @brief TCP socket for the connection. */
        boost::asio::ip::tcp::socket socket_;

        /** @brief Adapter for integrating with Boost.Asio. */
        AsioAdapterT ioAdapter_;
    };

#include "Interprocess/IConnectionHandler.hpp"

} // end namespace interprocess

#endif // interprocess_iconnection_handler_h
