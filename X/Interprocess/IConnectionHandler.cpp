#include "IConnectionHandler.h"

using interprocess::IConnectionHandler;

IConnectionHandler::IConnectionHandler(boost::asio::io_context& ioService) :
	ioServiceRef_(ioService),
	socket_(ioService),
	writeStrand_(ioService)
{
}

IConnectionHandler::~IConnectionHandler() = default;

boost::asio::ip::tcp::socket& IConnectionHandler::Socket()
{
	return socket_;
}

boost::asio::io_context& IConnectionHandler::IOService()
{
	return ioServiceRef_;
}

