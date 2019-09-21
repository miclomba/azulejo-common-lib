#ifndef entity_icliententity_h
#define entity_icliententity_h

#include <memory>
#include <string>

#include "config.h"

#include <boost/asio.hpp>

#include "Entity.h"

namespace entity {

class ENTITY_DLL_EXPORT IClientEntity : public Entity
{
public:
	IClientEntity();
	virtual ~IClientEntity();

	IClientEntity(const IClientEntity&) = delete;
	IClientEntity& operator=(const IClientEntity&) = delete;
	IClientEntity(IClientEntity&&) = delete;
	IClientEntity& operator=(IClientEntity&&) = delete;

	void Run(const std::string& host, const int port);

protected:
	virtual bool Work(std::shared_ptr<boost::asio::ip::tcp::socket> socket) = 0;

	const boost::asio::io_service* GetIOService() const;

private:
	std::shared_ptr<boost::asio::ip::tcp::socket> Connect(const std::string& host, const int port) const;

	mutable boost::asio::io_service ioService_;
};

} // namespace entity
#endif // entity_icliententity_h

