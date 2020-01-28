#include "AsyncServer.h"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "ConnectionHandler.h"

using boost::asio::ip::tcp;
using interprocess::AsyncServer;

AsyncServer::AsyncServer(const size_t numThreads) :
	numThreads_(numThreads),
	acceptor_(ioService_)
{
	if (numThreads_ == 0)
		throw std::runtime_error("Cannot construct AsyncServer because number of threads is invalid");

	// initialize threads here
}

AsyncServer::~AsyncServer() = default;

void AsyncServer::Start(const uint16_t port)
{
	tcp::endpoint endpoint(tcp::v4(), port);
	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(tcp::acceptor::reuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.listen();

	auto handler = std::make_shared<ConnectionHandler>(ioService_);
	acceptor_.async_accept(handler->Socket(), [=](auto ec) 
	{
		HandleNewConnection(handler, ec);
	});

	for (size_t i = 0; i < numThreads_; ++i)
		threadPool_.emplace_back([=]() { ioService_.run(); });
}

void AsyncServer::HandleNewConnection(AsyncServer::shared_conn_handler_t handler, const boost::system::error_code& ec)
{
	if (ec) return;

	handler->Start();

	auto newHandler = std::make_shared<ConnectionHandler>(ioService_);
	acceptor_.async_accept(newHandler->Socket(), [=](auto ec)
	{
		HandleNewConnection(newHandler, ec);
	});
}

size_t AsyncServer::GetNumThreads() const
{
	return numThreads_;
}

