#ifndef entity_iserverentity_h
#define entity_iserverentity_h

#include <memory>

#include "config.h"

#include <boost/asio.hpp>

#include "Entity.h"

namespace entity {

class ENTITY_DLL_EXPORT IServerEntity : public Entity
{
public:
	IServerEntity(const int port);
	virtual ~IServerEntity();

	IServerEntity(const IServerEntity&) = delete;
	IServerEntity& operator=(const IServerEntity&) = delete;
	IServerEntity(IServerEntity&&) = delete;
	IServerEntity& operator=(IServerEntity&&) = delete;

	void Run();

protected:
	virtual bool Work(std::shared_ptr<boost::asio::ip::tcp::socket> mySocket) = 0;

private:
	std::shared_ptr<boost::asio::ip::tcp::socket> Accept();

	boost::asio::io_service ioService_;
	boost::asio::ip::tcp::acceptor acceptor_;
};

} // end namespace entity
#endif // entity_iserverentity_h

