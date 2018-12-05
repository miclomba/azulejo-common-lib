
#include <array>
#include <iostream>
#include <exception>

#include <boost/array.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

#include "X/INetworkableEntity.h"

namespace
{
const std::string HOST = "localhost";
const int PORT = 13;
const int BUFF_LEN = 128;
const int SLEEP_TIME_MS = 15000;

std::string MakeString()
{
	return std::string("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
}

class Client : public global::INetworkableEntity
{
public:
	void Network() override
	{
		auto socket = Connect(HOST);

		for (;;)
		{
			boost::array<char, BUFF_LEN> buf;
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
	void Network() override
	{
		using namespace boost::asio::ip;

		for (;;)
		{
			auto socket = Accept(PORT);

			auto message = MakeString();

			boost::system::error_code ignored_error;
			boost::asio::write(*socket, boost::asio::buffer(message), ignored_error);
		}
	};
};
} // end namespace
/*
int main(const int argc, const char** argv)
{
	// server
	if (argc == 1)
	{
		std::cout << "Running server..." << std::endl;
		Server server;
		try
		{
			server.Network();
		}
		catch (std::exception&)
		{
			std::cout << "Server threw exception..." << std::endl;
		}
	}
	else
	{
		std::cout << "Running client..." << std::endl;

		// sleep some
		//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIME_MS));

		Client client;
		try
		{
			client.Network();
		}
		catch (std::exception&)
		{
			std::cout << "Client threw exception..." << std::endl;
		}
	}
}
*/
