#include "config.h"

#include <memory>
#include <stdexcept>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include "Interprocess/AsioAdapter.h"
#include "Interprocess/IConnectionHandler.h"
#include "Interprocess/TCPServer.h"

namespace errc = boost::system::errc;

using boost::asio::io_context;
using boost::asio::ip::basic_endpoint;
using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::system::error_category;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

using interprocess::AsioAdapter;
using interprocess::IConnectionHandler;
using interprocess::TCPServer;

namespace
{
const size_t TWO_THREADS = 2;
const size_t ZERO_THREADS = 0;
const uint16_t PORT = 3333;

using PODType = char;
using IOAdapter = AsioAdapter<PODType>;

struct MockHandler : public IConnectionHandler<PODType, IOAdapter> {
	MockHandler(io_context& context) : 
		IConnectionHandler(context) {}

	void StartApplication(IConnectionHandler::shared_conn_handler_t) override { ++startCount_; }

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

TEST(TCPServer, Construct)
{
	using Server = TCPServer<MockHandler, MockAcceptor>; 

	io_context context;
	EXPECT_NO_THROW(Server(context, TWO_THREADS));
}

TEST(TCPServer, ConstructThrowsWhenGivenZeroThreadCount)
{
	using Server = TCPServer<MockHandler, MockAcceptor>; 

	io_context context;
	EXPECT_THROW(Server(context, ZERO_THREADS), std::runtime_error);
}

TEST(TCPServer, Destroy)
{
	io_context context;

	EXPECT_FALSE(context.stopped());
	{
		TCPServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);
	}
	EXPECT_TRUE(context.stopped());
}

TEST(TCPServer, Join)
{
	io_context context;
	TCPServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);
	EXPECT_FALSE(context.stopped());
	EXPECT_NO_THROW(server.Join());
	EXPECT_TRUE(context.stopped());
}

TEST(TCPServer, JoinAfterStarting)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	TCPServer<MockHandler> server(context, TWO_THREADS);

	server.Start(endPoint);
	EXPECT_FALSE(context.stopped());
	EXPECT_NO_THROW(server.Join());
	EXPECT_TRUE(context.stopped());
}

TEST(TCPServer, GetNumThreads)
{
	io_context context;
	TCPServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);
	const size_t numThreads = server.GetNumThreads();
	EXPECT_EQ(numThreads, TWO_THREADS);
}

TEST(TCPServer, GetAcceptor)
{
	io_context context;
	TCPServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);
	EXPECT_NO_THROW(server.GetAcceptor());
}

TEST(TCPServer, Start)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	TCPServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);

	EXPECT_CALL(server.GetAcceptor(), async_accept(_, _)).
		Times(2).
		WillRepeatedly(Invoke(&server.GetAcceptor(), &MockAcceptor::AsyncAcceptImpl));

	EXPECT_NO_THROW(server.Start(endPoint));
}

TEST(TCPServer, AcceptConnection)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	TCPServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);

	EXPECT_CALL(server.GetAcceptor(), async_accept(_,_)).
		Times(2).
		WillRepeatedly(Invoke(&server.GetAcceptor(), &MockAcceptor::AsyncAcceptImpl));

	server.Start(endPoint);
}

TEST(TCPServer, HandleNewConnection)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	TCPServer<MockHandler, MockAcceptor> server(context);
	EXPECT_CALL(server.GetAcceptor(), async_accept(_, _)).
		Times(2).
		WillRepeatedly(Invoke(&server.GetAcceptor(), &MockAcceptor::AsyncAcceptImpl));

	MockHandler::ResetStartCount();
	server.Start(endPoint);
	EXPECT_EQ(MockHandler::GetStartCount(), 1);
}

TEST(TCPServer, HandleNewConnectionWithErrorCode)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	TCPServer<MockHandler, MockAcceptor> server(context);
	EXPECT_CALL(server.GetAcceptor(), async_accept(_, _)).
		Times(1).
		WillRepeatedly(Invoke(&server.GetAcceptor(), &MockAcceptor::AsyncAcceptImplWithErrorParam));

	MockHandler::ResetStartCount();
	server.Start(endPoint);
	EXPECT_EQ(MockHandler::GetStartCount(), 0);
}

