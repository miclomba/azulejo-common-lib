#ifndef interprocess_async_server_h
#define interprocess_async_server_h

#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include "config.h"

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

namespace interprocess {

template<typename ConnHandlerT>
class INTERPROCESS_DLL_EXPORT AsyncServer 
{
	using shared_conn_handler_t = std::shared_ptr<ConnHandlerT>;

public:
	AsyncServer(const size_t numThreads = 1);
	virtual ~AsyncServer();

	AsyncServer(const AsyncServer&) = delete;
	AsyncServer& operator=(const AsyncServer&) = delete;
	AsyncServer(AsyncServer&&) = delete;
	AsyncServer& operator=(AsyncServer&&) = delete;

	void Start(const uint16_t port);

	size_t GetNumThreads() const;

private:
	void HandleNewConnection(shared_conn_handler_t handler, const boost::system::error_code& ec);

	const size_t numThreads_;

	std::vector<std::thread> threadPool_;
	boost::asio::io_context ioService_;
	boost::asio::ip::tcp::acceptor acceptor_;
};

#include "AsyncServer.hpp"

} // end namespace interprocess
#endif // interprocess_async_server_h

