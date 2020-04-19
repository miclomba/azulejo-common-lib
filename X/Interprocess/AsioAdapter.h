#ifndef interprocess_asio_adapter_h
#define interprocess_asio_adapter_h

#include <functional>
#include <vector>

#include "config.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
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

	virtual void AsyncReadUntil(
		boost::asio::ip::tcp::socket& socket,
		boost::asio::streambuf& messageBuffer,
		const char UNTIL_CONDITION,
		std::function<void(boost::system::error_code error, size_t bytesTransferred)> handler
	);

	virtual void AsyncWrite(
		boost::asio::ip::tcp::socket& socket,
		boost::asio::mutable_buffer& messageBuffer,
		std::function<void(boost::system::error_code error, size_t)> handler
	);

	void AsyncConnect(
		boost::asio::ip::tcp::socket& socket,
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
		std::function<void(boost::system::error_code error, boost::asio::ip::tcp::resolver::iterator)> handler
	);
};

#include "AsioAdapter.hpp"

} // end namespace interprocess
#endif // interprocess_asio_adapter_h