
//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "config.h"
#include "chat_server.h"
#include "chat_client.h"
/*
int main(int argc, char* argv[])
{
	if (argc < 1)
	{
		std::cerr << "Usage: chat_server <port>\n";
		std::cerr << "Usage: chat_client <host> <port>\n";
		return 1;
	}

	if (std::string(argv[1]) == "chat_server")
	{
		try
		{
			if (argc < 3)
			{
				std::cerr << "Usage: chat_server <port>\n";
				return 1;
			}

			boost::asio::io_service io_service;
			tcp::endpoint endpoint(tcp::v6(), std::atoi(argv[2]));

			interprocess::AsyncServer<ServerChatHandler> server(io_service, 1);
			server.Start(endpoint);

			std::string input;
			std::cin >> input;
			//io_service.run();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}
	else if (std::string(argv[1]) == "chat_client")
	{
		if (argc != 4)
		{
			std::cerr << "Usage: chat_client <host> <port>\n";
			return 1;
		}

		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);
		auto endpoint_iterator = resolver.resolve({ argv[2], argv[3] });

		// handlers must be shared_ptrs
		auto client = std::make_shared<ClientChatHandler>(io_service);
		client->Connect(endpoint_iterator);

		std::thread t([&io_service]() { io_service.run(); });

		char line[chat_message::max_body_length + 1];
		while (std::cin.getline(line, chat_message::max_body_length + 1))
		{
			chat_message msg;
			msg.body_length(std::strlen(line));
			std::memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			client->write(msg);
		}

		client->Socket().close();
		t.join();
	}
}
*/
