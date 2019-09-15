#include "IServerEntity.h"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include <boost/asio.hpp>

namespace entity {

IServerEntity::IServerEntity(const int port)
{
	using boost::asio::ip::tcp;

	if (port < 0 || port > std::numeric_limits<unsigned short>::max())
		throw std::runtime_error("Cannot construct IServerEntity because port provided is invalid: " + std::to_string(port));

	ioService_ = std::make_shared<boost::asio::io_service>();
	acceptor_ = std::make_shared<tcp::acceptor>(*ioService_, tcp::endpoint(tcp::v4(), port));
}

IServerEntity::~IServerEntity() = default;

IServerEntity::IServerEntity(const IServerEntity&) = default;
IServerEntity& IServerEntity::operator=(const IServerEntity&) = default;
IServerEntity::IServerEntity(IServerEntity&&) = default;
IServerEntity& IServerEntity::operator=(IServerEntity&&) = default;

std::shared_ptr<boost::asio::ip::tcp::socket> IServerEntity::Accept()
{
	using boost::asio::ip::tcp;

	if (!ioService_ || !acceptor_)
		throw std::runtime_error("IServerEntity cannot accept connection because its members are nullptr");

	auto mySocket = std::make_shared<tcp::socket>(*ioService_);
	acceptor_->accept(*mySocket);
	return mySocket;
}

void IServerEntity::Run()
{
	using boost::asio::ip::tcp;

	try
	{
		bool doWork = true;
		while (doWork)
		{
			std::shared_ptr<tcp::socket> mySocket = Accept();
			doWork = Work(mySocket);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

} // namespace entity
