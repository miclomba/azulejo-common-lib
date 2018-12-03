#ifndef global_inetworkable_entity
#define global_inetworkable_entity

#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "Entity.h"

namespace global {

class INetworkableEntity : public Entity
{
public:
	INetworkableEntity();
	virtual ~INetworkableEntity();

	virtual void Network() = 0;

protected:
	void Connect(const std::string& url);

private:
	std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
	std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
};

} // namespace global
#endif // global_inetworkable_entity

