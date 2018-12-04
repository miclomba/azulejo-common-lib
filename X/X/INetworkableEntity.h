#ifndef global_inetworkable_entity
#define global_inetworkable_entity

#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT INetworkableEntity : public Entity
{
public:
	virtual ~INetworkableEntity();

	virtual void Network(const std::string& url) = 0;

protected:
	std::shared_ptr<boost::asio::ip::tcp::socket> Accept(const boost::asio::ip::tcp::endpoint& endPoint);
	std::shared_ptr<boost::asio::ip::tcp::socket> Connect(const std::string& url);
};

} // namespace global
#endif // global_inetworkable_entity

