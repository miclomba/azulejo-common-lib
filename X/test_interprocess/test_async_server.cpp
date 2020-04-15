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

#include "Interprocess/AsioAdapter.h"
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

using interprocess::AsioAdapter;
using interprocess::AsyncServer;
using interprocess::ConnectionHandler;

namespace
{
const size_t TWO_THREADS = 2;
const size_t ZERO_THREADS = 0;
const uint16_t PORT = 1500;

using PODType = char;

struct MockHandler : public ConnectionHandler<PODType> {
	MockHandler(io_context& context) : ConnectionHandler(context, ioAdapter_) {}
	void Start() { ++startCount_; }

	static void ResetStartCount() { startCount_ = 0; }
	static size_t GetStartCount() { return startCount_; }

private:
	static size_t startCount_;
	AsioAdapter<PODType> ioAdapter_;
};

size_t MockHandler::startCount_ = 0;

struct MockAcceptor : public tcp::acceptor
{
	MOCK_METHOD2(async_accept, void(tcp::socket&, std::function<void(const error_code&)>));

	MockAcceptor(io_context& context) : tcp::acceptor(context), context_(context) {}

	void AsyncAcceptImpl(tcp::socket& socket, std::function<void(const error_code&)> handler)
	{
		if (CalledAcceptMoreThanOnce()) return;
		handler(error_code());
	}

	void AsyncAcceptImplWithErrorParam(tcp::socket& socket, std::function<void(const error_code&)> handler)
	{
		if (CalledAcceptMoreThanOnce()) return;
		handler(errc::make_error_code(errc::not_supported));
	}

private:
	bool CalledAcceptMoreThanOnce() { return acceptCount_++ > 0; }
	size_t acceptCount_{ 0 };
	io_context& context_;
};

template<typename ConnHandlerT, typename ConnAcceptorT>
struct Server : public AsyncServer<MockHandler, MockAcceptor>
{
	Server(std::shared_ptr<ConnAcceptorT> acceptor) : AsyncServer(acceptor) {}
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
	EXPECT_FALSE(context.stopped());
	EXPECT_NO_THROW(server.Join());
	EXPECT_TRUE(context.stopped());
}

TEST(AsyncServer, JoinAfterStarting)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	AsyncServer<MockHandler> server(acceptor, TWO_THREADS);
	server.Start(PORT);
	EXPECT_FALSE(context.stopped());
	EXPECT_NO_THROW(server.Join());
	EXPECT_TRUE(context.stopped());
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
	AsyncServer<MockHandler, MockAcceptor> server(acceptor, TWO_THREADS);

	EXPECT_CALL(*acceptor, async_accept(_, _)).
		Times(2).
		WillRepeatedly(Invoke(&*acceptor, &MockAcceptor::AsyncAcceptImpl));

	EXPECT_NO_THROW(server.Start(PORT));
}

TEST(AsyncServer, AcceptConnection)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	AsyncServer<MockHandler, MockAcceptor> server(acceptor, TWO_THREADS);

	EXPECT_CALL(*acceptor, async_accept(_,_)).
		Times(2).
		WillRepeatedly(Invoke(&*acceptor, &MockAcceptor::AsyncAcceptImpl));

	server.Start(PORT);
}

TEST(AsyncServer, HandleNewConnection)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	EXPECT_CALL(*acceptor, async_accept(_, _)).
		Times(2).
		WillRepeatedly(Invoke(&*acceptor, &MockAcceptor::AsyncAcceptImpl));

	MockHandler::ResetStartCount();

	Server<MockHandler, MockAcceptor> server(acceptor);

	MockHandler::ResetStartCount();
	server.Start(PORT);
	EXPECT_EQ(MockHandler::GetStartCount(), 1);
}

TEST(AsyncServer, HandleNewConnectionWithErrorCode)
{
	io_context context;
	auto acceptor = std::make_shared<MockAcceptor>(context);
	EXPECT_CALL(*acceptor, async_accept(_, _)).
		Times(1).
		WillRepeatedly(Invoke(&*acceptor, &MockAcceptor::AsyncAcceptImplWithErrorParam));

	MockHandler::ResetStartCount();
	Server<MockHandler, MockAcceptor> server(acceptor);
	server.Start(PORT);
	EXPECT_EQ(MockHandler::GetStartCount(), 0);
}

