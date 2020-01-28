#include "ConnectionHandler.h"

using interprocess::ConnectionHandler;

ConnectionHandler::ConnectionHandler(boost::asio::io_context& ioService) :
	ioServiceRef_(ioService),
	socket_(ioService),
	writeStrand_(ioService)
{
}

ConnectionHandler::~ConnectionHandler() = default;

boost::asio::ip::tcp::socket& ConnectionHandler::Socket()
{
	return socket_;
}

void ConnectionHandler::Start()
{
}
