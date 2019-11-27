#include "IServer.h"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include <boost/asio.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;
using interprocess::IServer;

namespace
{
std::shared_ptr<boost::asio::ip::tcp::socket> SocketAccept(io_service& ioService_, tcp::acceptor& acceptor_)
{
	auto mySocket = std::make_shared<tcp::socket>(ioService_);
	acceptor_.accept(*mySocket);
	return mySocket;
}
} // end namespace

IServer::IServer(const int port) :
	acceptor_(ioService_, tcp::endpoint(tcp::v4(), port))
{
	if (port < 0 || port > std::numeric_limits<unsigned short>::max())
		throw std::runtime_error("Cannot construct IServer because port provided is invalid: " + std::to_string(port));
}

IServer::~IServer() = default;

void IServer::Run()
{
	try
	{
		bool doWork = true;
		while (doWork)
		{
			std::shared_ptr<tcp::socket> mySocket = SocketAccept(ioService_, acceptor_);
			doWork = Work(mySocket);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

const boost::asio::io_service* IServer::GetIOService() const
{
	return &ioService_;
}
