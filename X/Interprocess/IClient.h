#ifndef interprocess_iclient_h
#define interprocess_iclient_h

#include <memory>
#include <string>

#include "config.h"

#include <boost/asio.hpp>

namespace interprocess {

class INTERPROCESS_DLL_EXPORT IClient 
{
public:
	IClient();
	virtual ~IClient();

	IClient(const IClient&) = delete;
	IClient& operator=(const IClient&) = delete;
	IClient(IClient&&) = delete;
	IClient& operator=(IClient&&) = delete;

	void Run(const std::string& host, const int port);

protected:
	virtual bool Work(std::shared_ptr<boost::asio::ip::tcp::socket> socket) = 0;

	const boost::asio::io_service* GetIOService() const;

private:
	mutable boost::asio::io_service ioService_;
};

} // namespace interprocess
#endif // interprocess_iclient_h

