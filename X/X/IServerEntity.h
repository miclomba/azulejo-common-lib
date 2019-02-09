#ifndef entity_iserverentity_h
#define entity_iserverentity_h

#include <memory>

#include <boost/asio.hpp>

#include "config.h"
#include "Entity.h"

namespace entity {

class ENTITY_DLL_EXPORT IServerEntity : public Entity
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

} // end namespace entity
#endif // entity_iserverentity_h

