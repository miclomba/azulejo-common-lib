
#include "config.h"
#include "chat_server.h"
#include "chat_client.h"

#ifndef USER_DEFINED_MAIN
int fake_main(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
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
			
			std::string line;
			while (std::getline(std::cin, line))
			{
				if (c->HasReceivedMessages())
				{
					auto receivedMessage = c->GetOneMessage();
					for (char c : receivedMessage)
						std::cout << c;
					std::cout << '\n';
				}

				if (line.empty())
					continue;

				chat_message msg;
				msg.body_length(line.length());
				std::memcpy(msg.body(), line.c_str(), msg.body_length() + 1);
				msg.encode_header();
				c->write(msg);
			}
			

			c->Close();
			t.join();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}

	return 0;
}

