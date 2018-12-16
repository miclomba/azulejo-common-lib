#ifndef global_iclient_entity
#define global_iclient_entity

#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT IClientEntity : public Entity
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

} // namespace global
#endif // global_iclient_entity

