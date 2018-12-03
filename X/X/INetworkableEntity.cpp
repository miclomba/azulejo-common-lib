#include "INetworkableEntity.h"

#include <boost/asio.hpp>

namespace global {

INetworkableEntity::INetworkableEntity()
{
	using namespace boost::asio;
	using namespace boost::asio::ip;

	io_context ioContext;
	acceptor_ = std::make_shared<tcp::acceptor>(ioContext, tcp::endpoint(tcp::v4(), 13));
}

INetworkableEntity::~INetworkableEntity() = default;

void INetworkableEntity::Connect(const std::string& url)
{
	using namespace boost::asio;
	using namespace boost::asio::ip;

	if (socket_)
		throw std::runtime_error("Cannot connect on socket that is in use.");

	io_context ioContext;
	tcp::resolver resolver(ioContext);
	tcp::resolver::results_type endpoints = resolver.resolve(url, "daytime");

	socket_ = std::make_shared<tcp::socket>(ioContext);

	connect(*socket_, endpoints);
}

} // namespace global
