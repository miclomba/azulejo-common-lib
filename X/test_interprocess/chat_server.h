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
};

class chat_room 
{
public:
	void join(std::shared_ptr<chat_participant> participant)
	{
		participants_.insert(participant);
		for (auto msg: recent_msgs_)
			participant->PostOutgoingMessage(msg);
	}

	void leave(std::shared_ptr<chat_participant> participant)
	{
		participants_.erase(participant);
	}

	void deliver(const std::vector<char>& msg)
	{
		recent_msgs_.push_back(msg);
		while (recent_msgs_.size() > max_recent_msgs)
		  recent_msgs_.pop_front();

		for (auto participant : participants_)
			participant->PostOutgoingMessage(msg);
	}

	void run()
	{
		while (true)
		{
			for (auto participant : participants_)
			{
				if (participant->HasReceivedMessages())
					deliver(participant->GetOneMessage());
			}
		}
	}

private:
	std::set<std::shared_ptr<chat_participant>> participants_;
	const size_t max_recent_msgs = 100;
	std::deque<std::vector<char>> recent_msgs_;
};

chat_room globalRoom_;

class chat_session : public chat_participant
{
public:
	chat_session(boost::asio::io_context& io_context) : 
		chat_participant(io_context)
	{
	}

	void StartApplication(shared_conn_handler_t thisHandler) override
	{
		globalRoom_.join(std::dynamic_pointer_cast<chat_session>(thisHandler));
		PostReceiveMessages();
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
