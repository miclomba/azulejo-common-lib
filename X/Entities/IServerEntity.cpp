#include "IServerEntity.h"

#include <iostream>
#include <memory>

#include <boost/asio.hpp>

namespace entity {

IServerEntity::IServerEntity(const int port)
{
	using boost::asio::ip::tcp;

	if (port < 1)
		throw std::runtime_error("Cannot construct IServerEntity because port provided is < 1");

	ioService_ = std::make_shared<boost::asio::io_service>();
	acceptor_ = std::make_shared<tcp::acceptor>(*ioService_, tcp::endpoint(tcp::v4(), port));
}

IServerEntity::~IServerEntity() = default;

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
			auto mySocket = Accept();
			doWork = Work(mySocket);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

} // namespace entity
