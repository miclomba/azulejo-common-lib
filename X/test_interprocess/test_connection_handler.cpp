#include "config.h"

#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include "Interprocess/AsioAdapter.h"
#include "Interprocess/ConnectionHandler.h"

using boost::asio::io_context;
using boost::asio::ip::tcp;
using boost::asio::streambuf;
using boost::system::error_code;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

using interprocess::AsioAdapter;
using interprocess::ConnectionHandler;

namespace errc = boost::system::errc;

namespace
{
using PODType = char;

const std::string RECEIVED_MESSAGE = "message_received";
const uint16_t PORT = 1500;

struct IOAdapter : public AsioAdapter<PODType>
{
	MOCK_METHOD4(AsyncReadUntil, void(tcp::socket&, streambuf&, const char,
		std::function<void(const error_code& error, size_t bytesTransferred)>)
	);

	MOCK_METHOD3(AsyncWrite, void(tcp::socket&, boost::asio::mutable_buffer& message,
		std::function<void(const error_code& error, size_t bytesTransferred)>)
	);

	void AsyncReadUntilImpl(tcp::socket& socket, streambuf& inMessage, const char UNTIL_CONDITION,
		std::function<void(const error_code& error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledReadMoreThanOnce()) return;
		ReceiveMessage(inMessage);
		handlerWrapper(error_code(), 0); // place the handler in the executor without waiting
	}

	void AsyncWriteImpl(tcp::socket& socket, boost::asio::mutable_buffer& message,
		std::function<void(const error_code& error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledWriteMoreThanOnce()) return;
		handlerWrapper(error_code(), 0); // place the handler in the executor without waiting
	}

	void AsyncReadUntilImplWithErrorParam(tcp::socket& socket, streambuf& inMessage, const char UNTIL_CONDITION,
		std::function<void(const error_code& error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledReadMoreThanOnce()) return;
		ReceiveMessage(inMessage);
		handlerWrapper(errc::make_error_code(errc::not_supported), 0); // place the handler in the executor without waiting
	}

	void AsyncWriteImplWithErrorParam(tcp::socket& socket, boost::asio::mutable_buffer& message,
		std::function<void(const error_code& error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledWriteMoreThanOnce()) return;
		handlerWrapper(errc::make_error_code(errc::not_supported), 0); // place the handler in the executor without waiting
	}

private:
	bool CalledReadMoreThanOnce() { return readCount_++ > 0; }
	bool CalledWriteMoreThanOnce() { return writeCount_++ > 0; }

	void ReceiveMessage(streambuf& inMessage)
	{
		std::ostream stream(&inMessage);
		stream << RECEIVED_MESSAGE;
	}

	size_t readCount_{ 0 };
	size_t writeCount_{ 0 };
};

struct MockConnHandler : public ConnectionHandler<PODType> {
	MockConnHandler(io_context& context, const tcp::endpoint& endPoint, ::IOAdapter& ioAdapter) : 
		ConnectionHandler(context, endPoint, ioAdapter) {}
};
} // end namespace

TEST(ConnectionHandler, Construct)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	IOAdapter ioAdapter;
	EXPECT_NO_THROW(MockConnHandler handler(context, endPoint, ioAdapter));
}

TEST(ConnectionHandler, Socket)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	IOAdapter ioAdapter;
	MockConnHandler handler(context, endPoint, ioAdapter);

	tcp::socket& soket = handler.Socket();
	EXPECT_EQ(&(soket.get_io_context()), &context);
}

TEST(ConnectionHandler, IOService)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	IOAdapter ioAdapter;
	MockConnHandler handler(context, endPoint, ioAdapter);

	io_context& contextRef = handler.IOService();
	EXPECT_EQ(&(contextRef), &context);
}

TEST(ConnectionHandler, IOAdapter)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	auto ioAdapter = std::make_shared<AsioAdapter<PODType>>();
	auto handler = std::make_shared<ConnectionHandler<PODType>>(context, endPoint, *ioAdapter);

	EXPECT_EQ(&(handler->IOAdapter()),ioAdapter.get());
}

TEST(ConnectionHandler, PostReceiveMessages)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, endPoint, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncReadUntil(_, _, _, _)).
		Times(2).
		WillRepeatedly(
			DoAll(
				Invoke(ioAdapter.get(), &IOAdapter::AsyncReadUntilImpl)
			)
		);

	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
}

TEST(ConnectionHandler, PostReceiveMessagesWithError)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, endPoint, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncReadUntil(_, _, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(ioAdapter.get(), &IOAdapter::AsyncReadUntilImplWithErrorParam)
			)
		);

	EXPECT_FALSE(handler->HasReceivedMessages());
	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
	EXPECT_FALSE(handler->HasReceivedMessages());
}

TEST(ConnectionHandler, HasReceivedMessages)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, endPoint, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncReadUntil(_, _, _, _)).
		Times(2).
		WillRepeatedly(
			DoAll(
				Invoke(ioAdapter.get(), &IOAdapter::AsyncReadUntilImpl)
			)
		);

	EXPECT_FALSE(handler->HasReceivedMessages());
	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
	EXPECT_TRUE(handler->HasReceivedMessages());
}

TEST(ConnectionHandler, GetOneMessage)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, endPoint, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncReadUntil(_, _, _, _)).
		Times(2).
		WillRepeatedly(
			DoAll(
				Invoke(ioAdapter.get(), &IOAdapter::AsyncReadUntilImpl)
			)
		);

	EXPECT_FALSE(handler->HasReceivedMessages());
	
	handler->PostReceiveMessages();
	EXPECT_EQ(context.run(), 1);
	ASSERT_TRUE(handler->HasReceivedMessages());

	std::vector<PODType> message;
	EXPECT_NO_THROW(message = handler->GetOneMessage());

	for (size_t i = 0; i < message.size(); ++i)
		EXPECT_EQ(message[i], RECEIVED_MESSAGE[i]);
}

TEST(ConnectionHandler, GetOneMessageThrows)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, endPoint, *ioAdapter);

	ASSERT_FALSE(handler->HasReceivedMessages());
	EXPECT_THROW(handler->GetOneMessage(), std::runtime_error);
}

TEST(ConnectionHandler, PostOutgoingMessage)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, endPoint, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncWrite(_, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(ioAdapter.get(), &IOAdapter::AsyncWriteImpl)
			)
		);

	EXPECT_FALSE(handler->HasOutgoingMessages());

	std::vector<PODType> message(1, PODType('1'));
	EXPECT_NO_THROW(handler->PostOutgoingMessage(message));
	EXPECT_TRUE(handler->HasOutgoingMessages());

	EXPECT_EQ(context.run(), 1);
	EXPECT_FALSE(handler->HasOutgoingMessages());
}

TEST(ConnectionHandler, PostOutgoingMessageWithError)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, endPoint, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncWrite(_, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(ioAdapter.get(), &IOAdapter::AsyncWriteImplWithErrorParam)
			)
		);

	EXPECT_FALSE(handler->HasOutgoingMessages());

	std::vector<PODType> message(1, PODType('1'));
	EXPECT_NO_THROW(handler->PostOutgoingMessage(message));
	EXPECT_TRUE(handler->HasOutgoingMessages());

	EXPECT_EQ(context.run(), 1);
	EXPECT_TRUE(handler->HasOutgoingMessages());
}
