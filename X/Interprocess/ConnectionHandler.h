#ifndef interprocess_connection_handler_h
#define interprocess_connection_handler_h

#include <memory>

#include "config.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace interprocess {

class INTERPROCESS_DLL_EXPORT ConnectionHandler : public std::enable_shared_from_this<ConnectionHandler>
{
public:
	ConnectionHandler(boost::asio::io_context& ioService);

	virtual ~ConnectionHandler();
	ConnectionHandler(const ConnectionHandler&) = delete;
	ConnectionHandler& operator=(const ConnectionHandler&) = delete;
	ConnectionHandler(ConnectionHandler&&) = delete;
	ConnectionHandler& operator=(ConnectionHandler&&) = delete;

	virtual void Start();

	boost::asio::ip::tcp::socket& Socket();

private:
	boost::asio::io_context& ioServiceRef_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::io_context::strand writeStrand_;
};

} // end namespace interprocess
#endif // interprocess_iconnection_handler_h

