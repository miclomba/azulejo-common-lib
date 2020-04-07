#include "config.h"

#include <memory>
#include <stdexcept>
#include <string>

/*
	This test project uses gmock google project. To install gmock:
	from nuget:
		Install-Package gmock -Version 1.10.0
	then add in packages.config:
		<package id="gmock" version="1.10.0" targetFramework="native" />
	and add in vcxproj add to ExtensionTargets:
		<Import Project="..\packages\gmock.1.10.0\build\native\gmock.targets" Condition="Exists('..\packages\gmock.1.10.0\build\native\gmock.targets')" />
*/
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include "Interprocess/AsyncIO.h"
#include "Interprocess/AsyncServer.h"
#include "Interprocess/ConnectionHandler.h"

namespace errc = boost::system::errc;

using boost::asio::io_context;
using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::system::error_category;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

using interprocess::AsyncIO;
using interprocess::AsyncServer;
using interprocess::ConnectionHandler;

namespace
{
const size_t TWO_THREADS = 2;
const size_t ZERO_THREADS = 0;
const uint16_t PORT = 1500;

struct Packet : public std::string {};

struct MockHandler : public ConnectionHandler<Packet> {
	MockHandler(io_context& context) : ConnectionHandler(context, packetAsio_) {}
	void Start() { ++startCount_; }

	static void ResetStartCount() { startCount_ = 0; }
	static size_t GetStartCount() { return startCount_; }

private:
	static size_t startCount_;
	AsyncIO<Packet> packetAsio_;
};

size_t MockHandler::startCount_ = 0;

struct MockAcceptor : public tcp::acceptor
{
	MOCK_METHOD2(async_accept, void(tcp::socket&, std::function<void(error_code)>));

	MockAcceptor(io_context& context) : tcp::acceptor(context), context_(context) {}

	void AsyncAcceptImpl(tcp::socket& socket, std::function<void(error_code)> ec)
	{
		context_.post([this]() { this->Accept(); });
	}

	size_t GetAcceptCount() { return acceptCount_; }

private:
	void Accept() { ++acceptCount_; }
	size_t acceptCount_{ 0 };
	io_context& context_;
};

template<typename ConnHandlerT, typename ConnAcceptorT>
struct Server : public AsyncServer<MockHandler, MockAcceptor>
{
	using shared_conn_handler_t = std::shared_ptr<MockHandler>;

	Server(std::shared_ptr<ConnAcceptorT> acceptor) : AsyncServer(acceptor) {}
	void HandleNewConnection(shared_conn_handler_t handler, const error_code ec)
	{
		AsyncServer::HandleNewConnection(handler, ec);
	}
};

} // end namespace

TEST(AsyncServer, Construct)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	EXPECT_NO_THROW(AsyncServer<MockHandler> server(acceptor, TWO_THREADS));
}

TEST(AsyncServer, ConstructThrowsWhenGivenZeroThreadCount)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	EXPECT_THROW(AsyncServer<MockHandler> server(acceptor, ZERO_THREADS), std::runtime_error);
}

TEST(AsyncServer, ConstructThrowsWhenGivenNullAcceptor)
{
	EXPECT_THROW(AsyncServer<MockHandler> server(nullptr, TWO_THREADS), std::runtime_error);
}

TEST(AsyncServer, Join)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	AsyncServer<MockHandler> server(acceptor, TWO_THREADS);
	EXPECT_NO_THROW(server.Join());
}

TEST(AsyncServer, JoinAfterStarting)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	AsyncServer<MockHandler> server(acceptor, TWO_THREADS);
	server.Start(PORT);
	EXPECT_NO_THROW(server.Join());
}

TEST(AsyncServer, GetNumThreads)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	AsyncServer<MockHandler> server(acceptor, TWO_THREADS);
	const size_t numThreads = server.GetNumThreads();
	EXPECT_EQ(numThreads, TWO_THREADS);
}

TEST(AsyncServer, Start)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	AsyncServer<MockHandler> server(acceptor, TWO_THREADS);
	EXPECT_NO_THROW(server.Start(PORT));
}

TEST(AsyncServer, AcceptConnection)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	EXPECT_CALL(*acceptor, async_accept(_,_)).WillOnce(Invoke(&*acceptor, &MockAcceptor::AsyncAcceptImpl));

	AsyncServer<MockHandler, MockAcceptor> server(acceptor, TWO_THREADS);
	server.Start(PORT);

	EXPECT_EQ(acceptor->GetAcceptCount(), 1);
}

TEST(AsyncServer, HandleNewConnection)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	EXPECT_CALL(*acceptor, async_accept(_, _)).WillOnce(Invoke(&*acceptor, &MockAcceptor::AsyncAcceptImpl));

	MockHandler::ResetStartCount();
	EXPECT_EQ(MockHandler::GetStartCount(), 0);

	Server<MockHandler, MockAcceptor> server(acceptor);
	server.HandleNewConnection(std::make_shared<MockHandler>(context), error_code());

	EXPECT_EQ(MockHandler::GetStartCount(), 1);

	context.run();
	EXPECT_EQ(acceptor->GetAcceptCount(), 1);
}

TEST(AsyncServer, HandleNewConnectionWithErrorCode)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	ON_CALL(*acceptor, async_accept(_, _)).WillByDefault(Invoke(acceptor.get(), &MockAcceptor::AsyncAcceptImpl));

	MockHandler::ResetStartCount();
	EXPECT_EQ(MockHandler::GetStartCount(), 0);

	Server<MockHandler, MockAcceptor> server(acceptor);
	server.HandleNewConnection(std::make_shared<MockHandler>(context), errc::make_error_code(errc::not_supported));

	EXPECT_EQ(MockHandler::GetStartCount(), 0);
	EXPECT_EQ(acceptor->GetAcceptCount(), 0);
}

