#ifndef chat_handler_h
#define chat_handler_h

#include <ctime>
#include <deque>
#include <iostream>
#include <istream>
#include <memory>
#include <stdexcept>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>

#include "Interprocess/IConnectionHandler.h"

namespace networking {

class ChatHandler : interprocess::IConnectionHandler
{
public:
	void Start() override
	{
		ReceivePacket();
	}

private:

	void SendPacket(std::string message)
	{
		ioServiceRef_.post(writeStrand_.wrap(
			[me=shared_from_this(), message]() 
		{
			auto meDerived = static_cast<ChatHandler*>(me.get());
			meDerived->QueueMessage(message);
		}));
	}

	void QueueMessage(std::string message)
	{
		bool writeInProgress = !packetQue_.empty();
		packetQue_.push_back(std::move(message));

		if (!writeInProgress)
			SendPacketStart();
	}

	void SendPacketStart()
	{
		packetQue_.front() += UNTIL_CONDITION;
		boost::asio::async_write(Socket(), boost::asio::buffer(packetQue_.front()), 
			[me=shared_from_this()](const boost::system::error_code& error, size_t) 
		{
			auto meDerived = static_cast<ChatHandler*>(me.get());
			meDerived->SendPacketDone(error);
		});

	}

	void SendPacketDone(const boost::system::error_code& error)
	{
		if (error) return;

		packetQue_.pop_front();
		if (!packetQue_.empty())
			SendPacketStart();
	}

	void ReceivePacket()
	{
		boost::asio::async_read_until(Socket(), inPacket_, UNTIL_CONDITION, 
			[me=shared_from_this()](const boost::system::error_code& error, size_t bytesTransferred) 
		{
			auto meDerived = static_cast<ChatHandler*>(me.get());
			meDerived->ReceivePacketHandler(error, bytesTransferred);
		});
	}

	void ReceivePacketHandler(const boost::system::error_code& error, size_t bytesTransferred)
	{
		if (error) return;

		std::istream stream(&inPacket_);
		std::string packetString;
		stream >> packetString;

		// do something with the read packet

		ReceivePacket();
	}

	const char UNTIL_CONDITION = '\0';
	boost::asio::streambuf inPacket_;
	std::deque<std::string> packetQue_;
};

} // end networking
#endif // chat_handler_h

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
