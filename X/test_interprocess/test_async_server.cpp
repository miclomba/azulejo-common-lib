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
using boost::asio::ip::address;
using boost::asio::ip::basic_endpoint;
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
const uint16_t PORT = 3333;
const std::string RAW_IP_ADDRESS = "::1";
const address IP_ADDRESS = address::from_string(RAW_IP_ADDRESS);

using PODType = char;
using IOAdapter = AsioAdapter<PODType>;
struct Socket : public tcp::socket
{
	Socket(io_context& context, const tcp::endpoint& endPoint) : tcp::socket(context) {}
};

struct MockHandler : public ConnectionHandler<PODType, IOAdapter, Socket> {
	MockHandler(io_context& context, const tcp::endpoint& endPoint) : 
		ConnectionHandler(context, endPoint) {}

	void Start() { ++startCount_; }

	static void ResetStartCount() { startCount_ = 0; }
	static size_t GetStartCount() { return startCount_; }

private:
	static size_t startCount_;
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
} // end namespace

TEST(AsyncServer, Construct)
{
	using Server = AsyncServer<MockHandler, MockAcceptor>; 

	io_context context;
	EXPECT_NO_THROW(Server(context, TWO_THREADS));
}

TEST(AsyncServer, ConstructThrowsWhenGivenZeroThreadCount)
{
	using Server = AsyncServer<MockHandler, MockAcceptor>; 

	io_context context;
	EXPECT_THROW(Server(context, ZERO_THREADS), std::runtime_error);
}

TEST(AsyncServer, Destroy)
{
	io_context context;

	EXPECT_FALSE(context.stopped());
	{
		AsyncServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);
	}
	EXPECT_TRUE(context.stopped());
}

TEST(AsyncServer, Join)
{
	io_context context;
	AsyncServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);
	EXPECT_FALSE(context.stopped());
	EXPECT_NO_THROW(server.Join());
	EXPECT_TRUE(context.stopped());
}

TEST(AsyncServer, JoinAfterStarting)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	AsyncServer<MockHandler> server(context, TWO_THREADS);

	server.Start(endPoint);
	EXPECT_FALSE(context.stopped());
	EXPECT_NO_THROW(server.Join());
	EXPECT_TRUE(context.stopped());
}

TEST(AsyncServer, GetNumThreads)
{
	io_context context;
	AsyncServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);
	const size_t numThreads = server.GetNumThreads();
	EXPECT_EQ(numThreads, TWO_THREADS);
}

TEST(AsyncServer, GetAcceptor)
{
	io_context context;
	AsyncServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);
	EXPECT_NO_THROW(server.GetAcceptor());
}

TEST(AsyncServer, Start)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	AsyncServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);

	EXPECT_CALL(server.GetAcceptor(), async_accept(_, _)).
		Times(2).
		WillRepeatedly(Invoke(&server.GetAcceptor(), &MockAcceptor::AsyncAcceptImpl));

	EXPECT_NO_THROW(server.Start(endPoint));
}

TEST(AsyncServer, AcceptConnection)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	AsyncServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);

	EXPECT_CALL(server.GetAcceptor(), async_accept(_,_)).
		Times(2).
		WillRepeatedly(Invoke(&server.GetAcceptor(), &MockAcceptor::AsyncAcceptImpl));

	server.Start(endPoint);
}

TEST(AsyncServer, HandleNewConnection)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	AsyncServer<MockHandler, MockAcceptor> server(context);
	EXPECT_CALL(server.GetAcceptor(), async_accept(_, _)).
		Times(2).
		WillRepeatedly(Invoke(&server.GetAcceptor(), &MockAcceptor::AsyncAcceptImpl));

	MockHandler::ResetStartCount();
	server.Start(endPoint);
	EXPECT_EQ(MockHandler::GetStartCount(), 1);
}

TEST(AsyncServer, HandleNewConnectionWithErrorCode)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	AsyncServer<MockHandler, MockAcceptor> server(context);
	EXPECT_CALL(server.GetAcceptor(), async_accept(_, _)).
		Times(1).
		WillRepeatedly(Invoke(&server.GetAcceptor(), &MockAcceptor::AsyncAcceptImplWithErrorParam));

	MockHandler::ResetStartCount();
	server.Start(endPoint);
	EXPECT_EQ(MockHandler::GetStartCount(), 0);
}



//test that socket endpoint connection
/*
int main()
{
	io_context context;
	tcp::endpoint endPoint(IP_ADDRESS, PORT);

	boost::asio::ip::tcp::socket soc(context, endPoint);
	if (soc.is_open())
		std::cout << "socket is open on localhost" << std::endl;
}
*/
