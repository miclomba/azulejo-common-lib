#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H
//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "chat_message.hpp"

#include "Interprocess/IConnectionHandler.h"

using interprocess::IConnectionHandler;

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client : public IConnectionHandler<char> 
{
public:
	chat_client(boost::asio::io_context& io_context, const tcp::resolver::results_type& endpoints) : 
		IConnectionHandler(io_context) 
	{
		//Connect(endpoints);
		do_connect(endpoints);
	}

	void StartApplication(shared_conn_handler_t thisHandler)
	{
		do_read_header();
	}

	void write(const chat_message& msg)
	{
		boost::asio::post(IOService(),
			[this, msg]()
			{
				bool write_in_progress = !write_msgs_.empty();
				write_msgs_.push_back(msg);
				if (!write_in_progress)
				{
					do_write();
				}
			});
	}

	void close()
	{
		boost::asio::post(IOService(), [this]() { Socket().close(); });
	}

private:
	
	void do_connect(const tcp::resolver::results_type& endpoints)
	{
		boost::asio::async_connect(Socket(), endpoints,
			[this](boost::system::error_code ec, tcp::endpoint)
			{
			if (!ec)
			{
				do_read_header();
			}
			});
	}
	
	void do_read_header()
	{
		boost::asio::async_read(Socket(),
			boost::asio::buffer(read_msg_.data(), chat_message::header_length),
			[this](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec && read_msg_.decode_header())
				{
					do_read_body();
				}
				else
				{
					Socket().close();
				}
			});
	}

	void do_read_body()
	{
		boost::asio::async_read(Socket(),
			boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
				[this](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					std::cout.write(read_msg_.body(), read_msg_.body_length());
					std::cout << "\n";
					do_read_header();
				}
				else
				{
					Socket().close();
				}
			});
	}

	void do_write()
	{
		boost::asio::async_write(Socket(),
			boost::asio::buffer(write_msgs_.front().data(),
				write_msgs_.front().length()),
				[this](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					write_msgs_.pop_front();
					if (!write_msgs_.empty())
					{
						do_write();
					}
				}
				else
				{
					Socket().close();
				}
			});
	}

private:
	chat_message read_msg_;
	chat_message_queue write_msgs_;
};

#endif // CHAT_CLIENT_H
