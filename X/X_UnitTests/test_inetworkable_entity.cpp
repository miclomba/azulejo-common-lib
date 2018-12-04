#include "gtest/gtest.h"

#include <array>

#include <boost/array.hpp>

#include "X/INetworkableEntity.h"

namespace 
{
class Client : public global::INetworkableEntity
{
public:
	Client() {};

	void Network(const std::string& url) override
	{
		auto socket = Connect(url);

		for (;;)
		{
			boost::array<char, 128> buf;
			boost::system::error_code error;

			size_t len = socket->read_some(boost::asio::buffer(buf), error);

			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			std::cout.write(buf.data(), len);
		}
	};
};

class Server : public global::INetworkableEntity
{
public:
	Server() {};

	void Network(const std::string& url) override
	{
		using namespace boost::asio::ip;

		for (;;)
		{
			auto socket = Accept(tcp::endpoint(tcp::v4(), 13));

			boost::system::error_code ignored_error;
			boost::asio::write(*socket, boost::asio::buffer("received"), ignored_error);
		}
	};
};
} // end namespace

TEST(INetworkableEntity, ServerNetwork)
{
	Server server;
}