#include "IClient.h"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

#include <boost/asio.hpp>

using namespace boost::asio::error;

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::system::system_error;
using interprocess::IClient;

namespace
{
std::shared_ptr<tcp::socket> SocketConnect(const std::string& host, const int port, io_service& ioService)
{
	tcp::resolver resolvr(ioService);
	tcp::resolver::query qry(host, std::to_string(port));
	tcp::resolver::iterator endpoint_iterator = resolvr.resolve(qry);
	tcp::resolver::iterator end;

	auto mySocket = std::make_shared<tcp::socket>(ioService);

	error_code err = host_not_found;
	while (err && endpoint_iterator != end)
	{
		mySocket->close();
		mySocket->connect(*endpoint_iterator++, err);
	}
	if (err)
		throw system_error(err);

	return mySocket;
}
} // end namespace

IClient::IClient() = default;
IClient::~IClient() = default;

void IClient::Run(const std::string& host, const int port)
{
	if (host.empty())
		throw std::runtime_error("Cannot run IClient because host=''");
	if (port < 0 || port > std::numeric_limits<unsigned short>::max())
		throw std::runtime_error("Cannot run IClient because port is invalid: " + std::to_string(port));

	try
	{
		auto mySocket = SocketConnect(host, port, ioService_);
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

const boost::asio::io_service* IClient::GetIOService() const
{
	return &ioService_;
}
