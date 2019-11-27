#ifndef client_h
#define client_h

#include <iostream>
#include <memory>
#include <stdexcept>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "Interprocess/IClient.h"

namespace networking {

class Client : public interprocess::IClient
{
public:
	Client() = default;
	Client(const Client&) = delete;
	Client& operator=(const Client&) = delete;
	Client(Client&&) = delete;
	Client& operator=(Client&&) = delete;

	const boost::asio::io_service* GetIOServiceProtected() const 
	{ 
		return GetIOService(); 
	}

protected:
	bool Work(std::shared_ptr<boost::asio::ip::tcp::socket> mySocket) override
	{
		boost::array<char, 128> buf;
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

} // end networking
#endif // client_h

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
