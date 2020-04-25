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
					std::string messageStr(message.begin(), message.end());

					chat_message chatMessage;
					chatMessage.body_length(messageStr.length());
					std::memcpy(chatMessage.data() + HEADER_LENGTH, messageStr.data(), chatMessage.body_length() + 1);
					chatMessage.encode_header();

					deliver(chatMessage);
				}
			}
		}
	}

private:
	std::set<std::shared_ptr<chat_participant>> participants_;
	enum { max_recent_msgs = 100 };
	std::deque<chat_message> recent_msgs_;

  const size_t HEADER_LENGTH = 4;
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
		room_.join(std::dynamic_pointer_cast<chat_session>(shared_from_this()));
		PostReceiveMessages();
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

private:
	std::vector<char> ChatMessageToVector(const chat_message& msg)
	{
		size_t size = msg.length();
		std::string chatMessageStr(msg.data(), size + 1);
		return std::vector<char>(chatMessageStr.begin(), chatMessageStr.begin() + size);
	}
};

//----------------------------------------------------------------------

class chat_server : public interprocess::AsyncServer<chat_session>
{
public:
	chat_server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint, const size_t numThreads) : 
		AsyncServer<chat_session>(io_context, numThreads)
	{
		Start(endpoint);
	}
};

#endif // CHAT_SERVER_H
