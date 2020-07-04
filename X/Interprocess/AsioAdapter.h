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

template<typename PODType>
class AsioAdapter
{
public:
	AsioAdapter();
	virtual ~AsioAdapter();

	AsioAdapter(const AsioAdapter&) = delete;
	AsioAdapter& operator=(const AsioAdapter&) = delete;
	AsioAdapter(AsioAdapter&&) = delete;
	AsioAdapter& operator=(AsioAdapter&&) = delete;

	virtual void AsyncRead(
		boost::asio::ip::tcp::socket& socket,
		std::vector<PODType>& messageBuffer,
		const size_t bytesToRead,
		std::function<void(boost::system::error_code error, size_t bytesTransferred)> handler
	);

	virtual void AsyncWrite(
		boost::asio::ip::tcp::socket& socket,
		const std::vector<PODType>& messageBuffer,
		std::function<void(boost::system::error_code error, size_t)> handler
	);

	void AsyncConnect(
		boost::asio::ip::tcp::socket& socket,
		boost::asio::ip::tcp::resolver::results_type endpoint_iterator,
		std::function<void(boost::system::error_code error, boost::asio::ip::tcp::endpoint)> handler
	);
};

#include "AsioAdapter.hpp"

} // end namespace interprocess
#endif // interprocess_asio_adapter_h