#ifndef interprocess_iserver_h
#define interprocess_iserver_h

#include <memory>

#include "config.h"

#include <boost/asio.hpp>

namespace interprocess {

class INTERPROCESS_DLL_EXPORT IServer 
{
public:
	IServer(const int port);
	virtual ~IServer();

	IServer(const IServer&) = delete;
	IServer& operator=(const IServer&) = delete;
	IServer(IServer&&) = delete;
	IServer& operator=(IServer&&) = delete;

	void Run();

protected:
	virtual bool Work(std::shared_ptr<boost::asio::ip::tcp::socket> mySocket) = 0;

	const boost::asio::io_service* GetIOService() const;

private:
	boost::asio::io_service ioService_;
	boost::asio::ip::tcp::acceptor acceptor_;
};

} // end namespace interprocess
#endif // interprocess_iserver_h

