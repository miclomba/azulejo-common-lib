#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "chat_message.hpp"

#include "Interprocess/IConnectionHandler.h"

using interprocess::IConnectionHandler;

class chat_client : public IConnectionHandler<char> 
{
public:
	chat_client(boost::asio::io_context& io_context) :
		IConnectionHandler(io_context) 
	{
	}

	void write(const chat_message& msg)
	{
		PostOutgoingMessage(ChatMessageToVector(msg));
	}

private:
	std::vector<char> ChatMessageToVector(const chat_message& msg)
	{
		size_t size = msg.length();
		std::string chatMessageStr(msg.data(), size + 1);
		return std::vector<char>(chatMessageStr.begin(), chatMessageStr.begin() + size);
	}
};

#endif // CHAT_CLIENT_H
