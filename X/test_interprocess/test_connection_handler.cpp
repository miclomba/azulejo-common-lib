#include "config.h"

#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Interprocess/AsioAdapter.h"
#include "Interprocess/ConnectionHandler.h"

using boost::asio::io_context;
using boost::asio::ip::tcp;
using boost::asio::streambuf;
using boost::system::error_code;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

using interprocess::AsioAdapter;
using interprocess::ConnectionHandler;

namespace
{
using PODType = char;

void operator>>(std::istream& stream, std::vector<PODType>& message)
{
}

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

		// place the handler in the executor without waiting
		handlerWrapper(error_code(), 0);
	}

	void AsyncWriteImpl(tcp::socket& socket, boost::asio::mutable_buffer& message,
		std::function<void(const error_code& error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledWriteMoreThanOnce()) return;

		// place the handler in the executor without waiting
		handlerWrapper(error_code(), 0);
	}

	size_t GetReadCount() const
	{
		return readCount_;
	}

private:
	bool CalledReadMoreThanOnce() { return readCount_++ > 0; }
	bool CalledWriteMoreThanOnce() { return writeCount_++ > 0; }

	size_t readCount_{ 0 };
	size_t writeCount_{ 0 };
};

struct MockConnHandler : public ConnectionHandler<PODType> {
	MockConnHandler(io_context& context, ::IOAdapter& ioAdapter) : ConnectionHandler(context, ioAdapter) {}
};
} // end namespace

TEST(ConnectionHandler, Construct)
{
	io_context context;
	IOAdapter ioAdapter;
	EXPECT_NO_THROW(MockConnHandler handler(context, ioAdapter));
}

TEST(ConnectionHandler, Socket)
{
	io_context context;
	IOAdapter ioAdapter;
	MockConnHandler handler(context, ioAdapter);

	tcp::socket& soket = handler.Socket();
	EXPECT_EQ(&(soket.get_io_context()), &context);
}

TEST(ConnectionHandler, IOService)
{
	io_context context;
	IOAdapter ioAdapter;
	MockConnHandler handler(context, ioAdapter);

	io_context& contextRef = handler.IOService();
	EXPECT_EQ(&(contextRef), &context);
}

TEST(ConnectionHandler, IOAdapter)
{
	io_context context;
	auto ioAdapter = std::make_shared<AsioAdapter<PODType>>();
	auto handler = std::make_shared<ConnectionHandler<PODType>>(context, *ioAdapter);

	EXPECT_EQ(&(handler->IOAdapter()),ioAdapter.get());
}

TEST(ConnectionHandler, PostReceiveMessages)
{
	io_context context;
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(ioAdapter.get(), &IOAdapter::AsyncReadUntilImpl));

	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);

	// a second read indicates we completed one read cycle before forcing it to quit
	EXPECT_EQ(ioAdapter->GetReadCount(), 2);
}

TEST(ConnectionHandler, HasReceivedMessages)
{
	io_context context;
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(ioAdapter.get(), &IOAdapter::AsyncReadUntilImpl));

	EXPECT_FALSE(handler->HasReceivedMessages());
	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
	EXPECT_TRUE(handler->HasReceivedMessages());
}

TEST(ConnectionHandler, GetOneMessage)
{
	io_context context;
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(ioAdapter.get(), &IOAdapter::AsyncReadUntilImpl));

	EXPECT_FALSE(handler->HasReceivedMessages());
	
	handler->PostReceiveMessages();
	EXPECT_EQ(context.run(), 1);
	ASSERT_TRUE(handler->HasReceivedMessages());

	std::vector<PODType> message;
	EXPECT_NO_THROW(message = handler->GetOneMessage());
}

TEST(ConnectionHandler, GetOneMessageThrows)
{
	io_context context;
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(ioAdapter.get(), &IOAdapter::AsyncReadUntilImpl));

	ASSERT_FALSE(handler->HasReceivedMessages());
	EXPECT_THROW(handler->GetOneMessage(), std::runtime_error);
}

TEST(ConnectionHandler, PostOutgoingMessage)
{
	io_context context;
	auto ioAdapter = std::make_shared<IOAdapter>();
	auto handler = std::make_shared<MockConnHandler>(context, *ioAdapter);

	EXPECT_CALL(*ioAdapter, AsyncWrite(_, _, _)).WillRepeatedly(Invoke(ioAdapter.get(), &IOAdapter::AsyncWriteImpl));

	EXPECT_FALSE(handler->HasOutgoingMessages());

	std::vector<PODType> message(1, PODType('1'));
	EXPECT_NO_THROW(handler->PostOutgoingMessage(message));
	EXPECT_TRUE(handler->HasOutgoingMessages());

	EXPECT_EQ(context.run(), 1);
	EXPECT_FALSE(handler->HasOutgoingMessages());
}
