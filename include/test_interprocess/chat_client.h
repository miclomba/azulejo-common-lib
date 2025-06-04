#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "Interprocess/IConnectionHandler.h"

using interprocess::IConnectionHandler;

class chat_client : public IConnectionHandler<char> 
{
public:
	chat_client(boost::asio::io_context& io_context) :
		IConnectionHandler(io_context) 
	{
	}

	void StartApplication(shared_conn_handler_t thisHandler) override
	{
		PostReceiveMessages();
	}
};

#endif // CHAT_CLIENT_H
