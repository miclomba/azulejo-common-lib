#ifndef interprocess_async_server_h
#define interprocess_async_server_h

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

template<typename ConnHandlerT, typename ConnAcceptorT = boost::asio::ip::tcp::acceptor>
class AsyncServer 
{
	using shared_conn_handler_t = std::shared_ptr<ConnHandlerT>;

public:
	AsyncServer(std::shared_ptr<ConnAcceptorT> acceptor, const size_t numThreads = 1);
	virtual ~AsyncServer();

	AsyncServer(const AsyncServer&) = delete;
	AsyncServer& operator=(const AsyncServer&) = delete;
	AsyncServer(AsyncServer&&) = delete;
	AsyncServer& operator=(AsyncServer&&) = delete;

	void Start(const uint16_t port);
	void Join();

	size_t GetNumThreads() const;

private:
	void HandleNewConnection(shared_conn_handler_t handler, const boost::system::error_code ec);

	const size_t numThreads_;

	std::vector<std::thread> threadPool_;
	boost::asio::io_context* ioService_;
	std::shared_ptr<ConnAcceptorT> acceptor_;
};

#include "AsyncServer.hpp"

} // end namespace interprocess
#endif // interprocess_async_server_h

