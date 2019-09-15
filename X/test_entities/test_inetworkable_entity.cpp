#include "config.h"

#include <ctime>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

#include <gtest/gtest.h>
#include <boost/array.hpp>

#include "Entities/IClientEntity.h"
#include "Entities/IServerEntity.h"

namespace
{
const int SLEEP_TIME_MS = 15000;
const int PORT = 13;
const int BUFF_LEN = 128;
const std::string HOST = "localhost";

std::string MakeDaytimeString()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	char str[26];
	ctime_s(str, sizeof(str), &now);
	return str;
}

class Client : public entity::IClientEntity
{
protected:
	bool Work(std::shared_ptr<boost::asio::ip::tcp::socket> mySocket) override
	{
		boost::array<char, BUFF_LEN> buf;
		boost::system::error_code error;

		size_t len = mySocket->read_some(boost::asio::buffer(buf), error);
		if (error == boost::asio::error::eof)
			return false; // Connection closed cleanly by peer.
		else if (error)
			throw boost::system::system_error(error); // Some other error.

		std::cout.write(buf.data(), len);

		return true;
	}
};

class Server : public entity::IServerEntity
{
public:
	Server(const int port) : IServerEntity(port) {}
protected:
	bool Work(std::shared_ptr<boost::asio::ip::tcp::socket> mySocket) override
	{
		std::string message = MakeDaytimeString();

		boost::system::error_code ec;
		boost::asio::write(*mySocket, boost::asio::buffer(message), boost::asio::transfer_all(), ec);

		if (!ec)
			return true;
		return false;
	}
};
} // end namespace anonymous

TEST(IServerEntity, Construct)
{
	EXPECT_NO_THROW(Server server(PORT));
}

TEST(IServerEntity, ConstructThrowsWhenPortIsNegative)
{
	EXPECT_THROW(Server server(-1), std::runtime_error);
}

TEST(IServerEntity, ConstructThrowsWhenPortIsTooLarge)
{
	int port = std::numeric_limits<unsigned short>::max() + 1;
	EXPECT_THROW(Server server(port), std::runtime_error);
}

TEST(IClientEntity, Construct)
{
	EXPECT_NO_THROW(Client client());
}

TEST(IClientEntity, RunThrowsWhenHostIsEmpty)
{
	Client client;
	EXPECT_THROW(client.Run("", PORT), std::runtime_error);
}

TEST(IClientEntity, RunThrowsWhenPortIsNegative)
{
	Client client;
	EXPECT_THROW(client.Run(HOST,-1), std::runtime_error);
}

TEST(IClientEntity, RunThrowsWhenPortIsTooLarge)
{
	Client client;
	int port = std::numeric_limits<unsigned short>::max() + 1;
	EXPECT_THROW(client.Run(HOST,port), std::runtime_error);
}

/*
int main(const int argc, const char** argv)
{
	// server
	if (argc == 1)
	{
		std::cout << "Running server..." << std::endl;
		Server server(PORT);
		try
		{
			server.Run();
		}
		catch (std::exception&)
		{
			std::cout << "Server threw exception..." << std::endl;
		}
	}
	else
	{
		std::cout << "Running client..." << std::endl;

		Client client;
		try
		{
			client.Run(HOST, PORT);
		}
		catch (std::exception&)
		{
			std::cout << "Client threw exception..." << std::endl;
		}
	}
}
*/
