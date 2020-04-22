
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
				std::cerr << "Usage: chat_server <port> [<port> ...]\n";
				return 1;
			}

			boost::asio::io_context io_context;

			tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[2]));
			chat_server server(io_context, endpoint);

			io_context.run();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}
	else if (std::string(argv[1]) == "chat_client")
	{
		try
		{
			if (argc != 4)
			{
				std::cerr << "Usage: chat_client <host> <port>\n";
				return 1;
			}

			boost::asio::io_context io_context;

			tcp::resolver resolver(io_context);
			tcp::resolver::results_type endpoints = resolver.resolve(argv[2], argv[3]);
			auto c = std::make_shared<chat_client>(io_context); 
			c->Connect(endpoints);

			std::thread t([&io_context]() { io_context.run(); });

			char line[chat_message::max_body_length + 1];
			while (std::cin.getline(line, chat_message::max_body_length + 1))
			{
				chat_message msg;
				msg.body_length(std::strlen(line));
				std::memcpy(msg.body(), line, msg.body_length());
				msg.encode_header();
				c->write(msg);
			}

			c->close();
			t.join();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}
}

