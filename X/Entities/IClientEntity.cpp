#include "IClientEntity.h"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

#include <boost/asio.hpp>

namespace entity {

IClientEntity::IClientEntity()
{
	using boost::asio::ip::tcp;
	ioService_ = std::make_shared<boost::asio::io_service>();
}

IClientEntity::~IClientEntity() = default;

IClientEntity::IClientEntity(const IClientEntity&) = default;
IClientEntity& IClientEntity::operator=(const IClientEntity&) = default;
IClientEntity::IClientEntity(IClientEntity&&) = default;
IClientEntity& IClientEntity::operator=(IClientEntity&&) = default;

void IClientEntity::Run(const std::string& host, const int port)
{
	if (host.empty())
		throw std::runtime_error("Cannot run IClientEntity because host=''");
	if (port < 0 || port > std::numeric_limits<unsigned short>::max())
		throw std::runtime_error("Cannot run IClientEntity because port is invalid: " + std::to_string(port));

	try
	{
		auto mySocket = Connect(host, port);
		if (mySocket)
			while (Work(mySocket));
		else
			throw std::runtime_error("Could not connect to server at: " + host + ":" + std::to_string(port));
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

std::shared_ptr<boost::asio::ip::tcp::socket> IClientEntity::Connect(const std::string& host, const int port) const
{
	using namespace boost::asio;
	using namespace boost::asio::ip;

	tcp::resolver resolvr(*ioService_);

	tcp::resolver::query qry(host, std::to_string(port));

	tcp::resolver::iterator endpoint_iterator = resolvr.resolve(qry);
	tcp::resolver::iterator end;

	auto mySocket = std::make_shared<tcp::socket>(*ioService_);

	boost::system::error_code error = boost::asio::error::host_not_found;
	while (error && endpoint_iterator != end)
	{
		mySocket->close();
		mySocket->connect(*endpoint_iterator++, error);
	}
	if (error)
		throw boost::system::system_error(error);

	return mySocket;
}

} // namespace entity
