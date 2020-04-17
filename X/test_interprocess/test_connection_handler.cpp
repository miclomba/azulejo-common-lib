#include "config.h"

#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include "Interprocess/AsioAdapter.h"
#include "Interprocess/IConnectionHandler.h"

using boost::asio::io_context;
using boost::asio::ip::address;
using boost::asio::ip::tcp;
using boost::asio::streambuf;
using boost::system::error_code;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

using interprocess::AsioAdapter;
using interprocess::IConnectionHandler;

namespace errc = boost::system::errc;

namespace
{
using PODType = char;

const std::string RECEIVED_MESSAGE = "message_received";
const std::string SENT_MESSAGE = "message_sent";
const char UNTIL_SYMBOL = '\0';
const uint16_t PORT = 3333;
const std::string RAW_IP_ADDRESS = "::1";
const address IP_ADDRESS = address::from_string(RAW_IP_ADDRESS);

struct Socket : public tcp::socket
{
	Socket(io_context& context, const tcp::endpoint& endPoint) : tcp::socket(context) {}
};

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
		ValidateSentMessage(message);
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
		ValidateSentMessage(message);
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

	void ValidateSentMessage(boost::asio::mutable_buffer& message)
	{
		size_t podSize = sizeof(PODType);
		std::vector<PODType> messageVector(message.size() / podSize);
		boost::asio::buffer_copy(boost::asio::buffer(messageVector), message);
		EXPECT_EQ(messageVector.size(), SENT_MESSAGE.size() + 1);
		size_t i = 0;
		for (i = 0; i < SENT_MESSAGE.size(); ++i)
			EXPECT_EQ(messageVector[i], SENT_MESSAGE.at(i));
		EXPECT_EQ(messageVector[i], UNTIL_SYMBOL);
	}

	size_t readCount_{ 0 };
	size_t writeCount_{ 0 };
};

struct MockConnHandler : public IConnectionHandler<PODType, IOAdapter, Socket>	
{
	MockConnHandler(io_context& context, const tcp::endpoint& endPoint) : 
		IConnectionHandler(context, endPoint) {}
	void StartApplication() override {}
};
} // end namespace

TEST(IConnectionHandler, ConstructForServer)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	EXPECT_NO_THROW(MockConnHandler handler(context, endPoint));
}

TEST(IConnectionHandler, ConstructForClient)
{
	io_context context;
	tcp::endpoint endPoint(IP_ADDRESS, PORT);
	EXPECT_NO_THROW(MockConnHandler handler(context, endPoint));
}

TEST(IConnectionHandler, Socket)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	MockConnHandler handler(context, endPoint);

	tcp::socket& soket = handler.Socket();
	EXPECT_EQ(&(soket.get_io_context()), &context);
}

TEST(IConnectionHandler, IOService)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	MockConnHandler handler(context, endPoint);

	io_context& contextRef = handler.IOService();
	EXPECT_EQ(&(contextRef), &context);
}

TEST(IConnectionHandler, IOAdapter)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	MockConnHandler handler(context, endPoint);

	EXPECT_NO_THROW(handler.IOAdapter());
}

TEST(IConnectionHandler, PostReceiveMessages)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context, endPoint);

	EXPECT_CALL(handler->IOAdapter(), AsyncReadUntil(_, _, _, _)).
		Times(2).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadUntilImpl)
			)
		);

	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
}

TEST(IConnectionHandler, PostReceiveMessagesWithError)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context, endPoint);

	EXPECT_CALL(handler->IOAdapter(), AsyncReadUntil(_, _, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadUntilImplWithErrorParam)
			)
		);

	EXPECT_FALSE(handler->HasReceivedMessages());
	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
	EXPECT_FALSE(handler->HasReceivedMessages());
}

TEST(IConnectionHandler, HasReceivedMessages)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context, endPoint);

	EXPECT_CALL(handler->IOAdapter(), AsyncReadUntil(_, _, _, _)).
		Times(2).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadUntilImpl)
			)
		);

	EXPECT_FALSE(handler->HasReceivedMessages());
	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
	EXPECT_TRUE(handler->HasReceivedMessages());
}

TEST(IConnectionHandler, GetOneMessage)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context, endPoint);

	EXPECT_CALL(handler->IOAdapter(), AsyncReadUntil(_, _, _, _)).
		Times(2).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadUntilImpl)
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

TEST(IConnectionHandler, GetOneMessageThrows)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);
	MockConnHandler handler(context, endPoint);

	ASSERT_FALSE(handler.HasReceivedMessages());
	EXPECT_THROW(handler.GetOneMessage(), std::runtime_error);
}

TEST(IConnectionHandler, PostOutgoingMessage)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context, endPoint);

	EXPECT_CALL(handler->IOAdapter(), AsyncWrite(_, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncWriteImpl)
			)
		);

	EXPECT_FALSE(handler->HasOutgoingMessages());

	std::vector<PODType> message(SENT_MESSAGE.cbegin(), SENT_MESSAGE.cend());
	EXPECT_NO_THROW(handler->PostOutgoingMessage(message));
	EXPECT_TRUE(handler->HasOutgoingMessages());

	EXPECT_EQ(context.run(), 1);
	EXPECT_FALSE(handler->HasOutgoingMessages());
}

TEST(IConnectionHandler, PostOutgoingMessageWithError)
{
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context, endPoint);

	EXPECT_CALL(handler->IOAdapter(), AsyncWrite(_, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncWriteImplWithErrorParam)
			)
		);

	EXPECT_FALSE(handler->HasOutgoingMessages());

	std::vector<PODType> message(SENT_MESSAGE.cbegin(), SENT_MESSAGE.cend());
	EXPECT_NO_THROW(handler->PostOutgoingMessage(message));
	EXPECT_TRUE(handler->HasOutgoingMessages());

	EXPECT_EQ(context.run(), 1);
	EXPECT_TRUE(handler->HasOutgoingMessages());
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
