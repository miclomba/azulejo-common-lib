#ifndef entity_icliententity_h
#define entity_icliententity_h

#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "config.h"
#include "Entity.h"

namespace entity {

class ENTITY_DLL_EXPORT IClientEntity : public Entity
{
public:
	IClientEntity();
	virtual ~IClientEntity();

	void Run(const std::string& host, const int port);

protected:
	virtual bool Work(std::shared_ptr<boost::asio::ip::tcp::socket> socket) = 0;

private:
	std::shared_ptr<boost::asio::ip::tcp::socket> Connect(const std::string& host, const int port) const;

	std::shared_ptr<boost::asio::io_service> ioService_;
};

} // namespace entity
#endif // entity_icliententity_h

