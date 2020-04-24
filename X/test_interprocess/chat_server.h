#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>

#include "chat_message.hpp"

#include "Interprocess/AsyncServer.h"
#include "Interprocess/IConnectionHandler.h"

using boost::asio::ip::tcp;

class chat_participant : public interprocess::IConnectionHandler<char>
{
public:
	chat_participant(boost::asio::io_context& io_context) :
		interprocess::IConnectionHandler<char>(io_context)
	{
	}

	virtual void deliver(const chat_message& msg) = 0;
};

class chat_room 
{
public:
	void join(std::shared_ptr<chat_participant> participant)
	{
		participants_.insert(participant);
		for (auto msg: recent_msgs_)
			participant->deliver(msg);
	}

	void leave(std::shared_ptr<chat_participant> participant)
	{
		participants_.erase(participant);
	}

	void deliver(const chat_message& msg)
	{
		recent_msgs_.push_back(msg);
		while (recent_msgs_.size() > max_recent_msgs)
		  recent_msgs_.pop_front();

		for (auto participant : participants_)
			participant->deliver(msg);
	}

	void run()
	{
		while (true)
		{
			for (auto participant : participants_)
			{
				if (participant->HasReceivedMessages())
				{
					auto message = participant->GetOneMessage();

					chat_message chatMessage;
					chatMessage.body_length(message.size());
					std::memcpy(chatMessage.data(), message.data(), message.size());

					deliver(chatMessage);
				}
			}
		}
	}

private:
  std::set<std::shared_ptr<chat_participant>> participants_;
  enum { max_recent_msgs = 100 };
  std::deque<chat_message> recent_msgs_;
};

chat_room globalRoom_;

//----------------------------------------------------------------------

class chat_session : public chat_participant
{
public:
	chat_session(boost::asio::io_context& io_context) : 
		chat_participant(io_context),
		room_(globalRoom_)
	{
	}

	void StartApplication(shared_conn_handler_t thisHandler) override
	{
		start();
	}

  void start()
  {
	room_.join(std::dynamic_pointer_cast<chat_session>(shared_from_this()));
	do_read_header();
	//PostReceiveMessages();
  }

  void deliver(const chat_message& msg)
  {
	bool write_in_progress = !write_msgs_.empty();
	write_msgs_.push_back(msg);
	if (!write_in_progress)
	{
	  do_write();
	}
  }

private:
  void do_read_header()
  {
	auto self(shared_from_this());
	boost::asio::async_read(Socket(),
		boost::asio::buffer(read_msg_.data(), chat_message::header_length),
		[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
		  if (!ec && read_msg_.decode_header())
		  {
			do_read_body();
		  }
		  else
		  {
			room_.leave(std::dynamic_pointer_cast<chat_session>(shared_from_this()));
		  }
		});
  }

  void do_read_body()
  {
	auto self(shared_from_this());
	boost::asio::async_read(Socket(),
		boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
		[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
		  if (!ec)
		  {
			room_.deliver(read_msg_);
			do_read_header();
		  }
		  else
		  {
			room_.leave(std::dynamic_pointer_cast<chat_session>(shared_from_this()));
		  }
		});
  }
  
  void do_write()
  {
	auto self(shared_from_this());
	boost::asio::async_write(Socket(),
		boost::asio::buffer(write_msgs_.front().data(),
		  write_msgs_.front().length()),
		[this, self](boost::system::error_code ec, std::size_t)
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
			room_.leave(std::dynamic_pointer_cast<chat_session>(shared_from_this()));
		  }
		});
  }

  chat_room& room_;
  chat_message read_msg_;
  std::deque<chat_message> write_msgs_;
};

//----------------------------------------------------------------------

class chat_server : public interprocess::AsyncServer<chat_session>
{
public:
	chat_server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint) : 
		AsyncServer<chat_session>(io_context)
	{
		Start(endpoint);
	}
};

#endif // CHAT_SERVER_H
