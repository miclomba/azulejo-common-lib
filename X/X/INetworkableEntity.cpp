#include "INetworkableEntity.h"

#include <memory>

#include <boost/asio.hpp>

namespace global {

INetworkableEntity::~INetworkableEntity() = default;

std::shared_ptr<boost::asio::ip::tcp::socket> INetworkableEntity::Accept(const boost::asio::ip::tcp::endpoint& endPoint)
{
	using namespace boost::asio;
	using namespace boost::asio::ip;

	io_context ioContext;

	tcp::acceptor acceptor(ioContext, endPoint);
	
	auto socket = std::make_shared<tcp::socket>(ioContext);
	acceptor.accept(*socket);

	return socket;
}

std::shared_ptr<boost::asio::ip::tcp::socket> INetworkableEntity::Connect(const std::string& url)
{
	using namespace boost::asio;
	using namespace boost::asio::ip;

	io_context ioContext;
	tcp::resolver resolver(ioContext);
	tcp::resolver::results_type endpoints = resolver.resolve(url, "daytime");

	auto socket = std::make_shared<tcp::socket>(ioContext);

	connect(*socket, endpoints);

	return socket;
}

} // namespace global
