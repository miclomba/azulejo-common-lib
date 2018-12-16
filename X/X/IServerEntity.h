#ifndef global_iserver_entity
#define global_iserver_entity

#include <memory>

#include <boost/asio.hpp>

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT IServerEntity : public Entity
{
public:
	IServerEntity(const int port);
	virtual ~IServerEntity();

	void Run();

protected:
	virtual bool Work(std::shared_ptr<boost::asio::ip::tcp::socket> mySocket) = 0;

private:
	std::shared_ptr<boost::asio::ip::tcp::socket> Accept();

	std::shared_ptr<boost::asio::io_service> ioService_;
	std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
};

} // namespace global
#endif // global_iserver_entity

