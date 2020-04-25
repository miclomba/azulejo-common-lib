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
};

#endif // CHAT_CLIENT_H
