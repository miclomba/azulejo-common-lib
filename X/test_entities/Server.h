#ifndef server_h
#define server_h

#include <ctime>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "Entities/IServerEntity.h"

namespace networking {

class Server : public entity::IServerEntity
{
public:
	Server(const int port) : IServerEntity(port) {}

	Server(const Server&) = default;
	Server& operator=(const Server&) = default;
	Server(Server&&) = default;
	Server& operator=(Server&&) = default;

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

private:
	std::string MakeDaytimeString()
	{
		using namespace std; // For time_t, time and ctime;
		time_t now = time(0);
		char str[26];
		ctime_s(str, sizeof(str), &now);
		return str;
	}
};

} // end networking
#endif // server_h

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
