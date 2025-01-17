/**
 * @file AsioAdapter.h
 * @brief Declaration of the AsioAdapter class template for handling Boost.Asio-based network operations.
 */

#ifndef interprocess_asio_adapter_h
#define interprocess_asio_adapter_h

#include <functional>
#include <vector>

#include "config.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>

namespace interprocess {

/**
 * @class AsioAdapter
 * @brief A class template for adapting Boost.Asio networking functionality to handle POD (Plain Old Data) types.
 *
 * @tparam PODType The data type for the messages handled by the adapter.
 */
template<typename PODType>
class AsioAdapter
{
public:
    /**
     * @brief Default constructor for the AsioAdapter class.
     */
    AsioAdapter();

    /**
     * @brief Virtual destructor for the AsioAdapter class.
     */
    virtual ~AsioAdapter();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    AsioAdapter(const AsioAdapter&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     * @return Reference to the updated instance (not used).
     */
    AsioAdapter& operator=(const AsioAdapter&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving.
     */
    AsioAdapter(AsioAdapter&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving.
     * @return Reference to the updated instance (not used).
     */
    AsioAdapter& operator=(AsioAdapter&&) = delete;

    /**
     * @brief Perform an asynchronous read operation.
     * @param socket The Boost.Asio socket to read from.
     * @param messageBuffer The buffer to store the read data.
     * @param bytesToRead The number of bytes to read.
     * @param handler The callback function to invoke upon completion.
     */
    virtual void AsyncRead(
        boost::asio::ip::tcp::socket& socket,
        std::vector<PODType>& messageBuffer,
        const size_t bytesToRead,
        std::function<void(boost::system::error_code error, size_t bytesTransferred)> handler
    );

    /**
     * @brief Perform an asynchronous write operation.
     * @param socket The Boost.Asio socket to write to.
     * @param messageBuffer The buffer containing the data to write.
     * @param handler The callback function to invoke upon completion.
     */
    virtual void AsyncWrite(
        boost::asio::ip::tcp::socket& socket,
        const std::vector<PODType>& messageBuffer,
        std::function<void(boost::system::error_code error, size_t)> handler
    );

    /**
     * @brief Perform an asynchronous connection operation.
     * @param socket The Boost.Asio socket to connect.
     * @param endpoint_iterator The iterator containing the endpoints to connect to.
     * @param handler The callback function to invoke upon connection.
     */
    void AsyncConnect(
        boost::asio::ip::tcp::socket& socket,
        boost::asio::ip::tcp::resolver::results_type endpoint_iterator,
        std::function<void(boost::system::error_code error, boost::asio::ip::tcp::endpoint)> handler
    );
};

#include "AsioAdapter.hpp"

} // end namespace interprocess

#endif // interprocess_asio_adapter_h