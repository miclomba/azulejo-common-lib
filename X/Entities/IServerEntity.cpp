#include "IServerEntity.h"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace entity {

IServerEntity::IServerEntity(const int port) :
	acceptor_(ioService_, tcp::endpoint(tcp::v4(), port))
{
	if (port < 0 || port > std::numeric_limits<unsigned short>::max())
		throw std::runtime_error("Cannot construct IServerEntity because port provided is invalid: " + std::to_string(port));
}

IServerEntity::~IServerEntity() = default;

std::shared_ptr<boost::asio::ip::tcp::socket> IServerEntity::Accept()
{
	auto mySocket = std::make_shared<tcp::socket>(ioService_);
	acceptor_.accept(*mySocket);
	return mySocket;
}

void IServerEntity::Run()
{
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
