#ifndef interprocess_async_io_h
#define interprocess_async_io_h

#include <functional>
#include <vector>

#include "config.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>

namespace interprocess {

template<typename PacketT>
class AsyncIO
{
public:
	AsyncIO();
	virtual ~AsyncIO();

	AsyncIO(const AsyncIO&) = delete;
	AsyncIO& operator=(const AsyncIO&) = delete;
	AsyncIO(AsyncIO&&) = delete;
	AsyncIO& operator=(AsyncIO&&) = delete;

	virtual void AsyncReadUntil(
		boost::asio::ip::tcp::socket& socket,
		boost::asio::streambuf& inPacket,
		const char UNTIL_CONDITION,
		std::function<void(const boost::system::error_code& error, size_t bytesTransferred)> handler
	);

	virtual void AsyncWrite(
		boost::asio::ip::tcp::socket& socket,
		std::vector<PacketT>& packet,
		std::function<void(const boost::system::error_code& error, size_t)> handler
	);
};

#include "AsyncIO.hpp"

} // end namespace interprocess
#endif // interprocess_async_io_h