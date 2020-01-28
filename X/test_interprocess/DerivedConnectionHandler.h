#ifndef connection_handler_h
#define connection_handler_h

#include <ctime>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "Interprocess/ConnectionHandler.h"

namespace networking {

struct DerivedConnectionHanlder : interprocess::ConnectionHandler
{
	void Start() override
	{
	}
};

} // end networking
#endif // connection_handler_h

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
