#ifndef interprocess_iconnection_handler_h
#define interprocess_iconnection_handler_h

#include <memory>

#include "config.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace interprocess {

class INTERPROCESS_DLL_EXPORT IConnectionHandler : public std::enable_shared_from_this<IConnectionHandler>
{
public:
	IConnectionHandler(boost::asio::io_context& ioService);

	virtual ~IConnectionHandler();
	IConnectionHandler(const IConnectionHandler&) = delete;
	IConnectionHandler& operator=(const IConnectionHandler&) = delete;
	IConnectionHandler(IConnectionHandler&&) = delete;
	IConnectionHandler& operator=(IConnectionHandler&&) = delete;

	virtual void Start() = 0;

	boost::asio::ip::tcp::socket& Socket();
	boost::asio::io_context& IOService();

protected:
	boost::asio::io_context& ioServiceRef_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::io_context::strand writeStrand_;
};

} // end namespace interprocess
#endif // interprocess_iconnection_handler_h

