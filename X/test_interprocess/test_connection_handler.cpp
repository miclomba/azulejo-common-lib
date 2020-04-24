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
using boost::asio::ip::basic_resolver_results;
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

const std::string RECEIVED_MESSAGE = "   7message";
const std::string RECEIVED_BAD_MESSAGE = "    message";
const std::string SENT_MESSAGE = "message_sent";
const std::string SENT_MESSAGE_WITH_HEADER = "  12message_sent";
const size_t HEADER_LENGTH = 4;
const uint16_t PORT = 3333;
const std::string RAW_IP_ADDRESS = "::1";
const address IP_ADDRESS = address::from_string(RAW_IP_ADDRESS);

struct IOAdapter : public AsioAdapter<PODType>
{
	MOCK_METHOD4(AsyncRead, void(tcp::socket&, std::vector<char>&, const size_t,
		std::function<void(error_code error, size_t bytesTransferred)>)
	);

	MOCK_METHOD3(AsyncWrite, void(tcp::socket&, boost::asio::mutable_buffer& message,
		std::function<void(error_code error, size_t bytesTransferred)>)
	);

	MOCK_METHOD3(AsyncConnect, void(tcp::socket&, tcp::resolver::results_type endPoints,
		std::function<void(error_code error, tcp::endpoint endPoint)>)
	);

	void AsyncReadImpl(tcp::socket& socket, std::vector<char>& inMessage, const size_t length,
		std::function<void(error_code error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledReadTooManyTimes()) return;
		ReceiveMessage(inMessage, RECEIVED_MESSAGE);
		// place the handler in the executor without waiting
		handlerWrapper(error_code(), readCount_ == 1 ? HEADER_LENGTH : RECEIVED_MESSAGE.size()); 
	}

	void AsyncWriteImpl(tcp::socket& socket, boost::asio::mutable_buffer& message,
		std::function<void(error_code error, size_t bytesTransferred)>&& handlerWrapper)
	{
		ValidateSentMessage(message);
		handlerWrapper(error_code(), 0); // place the handler in the executor without waiting
	}

	void AsyncConnectImpl(tcp::socket& socket, tcp::resolver::results_type endPoints,
		std::function<void(error_code error, tcp::endpoint endPoint)>&& handler)
	{
		ValidateEndPoint(endPoints);
		handler(error_code(), endPoints.begin()->endpoint()); 
	}

	void AsyncReadImplWithErrorParamReadingHeader(tcp::socket& socket, std::vector<char>& inMessage, const size_t length,
		std::function<void(error_code error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledReadTooManyTimes()) return;
		ReceiveMessage(inMessage, RECEIVED_MESSAGE);
		handlerWrapper(errc::make_error_code(errc::not_supported), HEADER_LENGTH); // place the handler in the executor without waiting
	}

	void AsyncReadImplWithErrorParamReadingHeaderLength(tcp::socket& socket, std::vector<char>& inMessage, const size_t length,
		std::function<void(error_code error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledReadTooManyTimes()) return;
		ReceiveMessage(inMessage, RECEIVED_BAD_MESSAGE);
		handlerWrapper(error_code(), HEADER_LENGTH); // place the handler in the executor without waiting
	}

	void AsyncReadImplWithErrorParamReadingBody(tcp::socket& socket, std::vector<char>& inMessage, const size_t length,
		std::function<void(error_code error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledReadTooManyTimes()) return;
		ReceiveMessage(inMessage, RECEIVED_MESSAGE);
		handlerWrapper(
			readCount_ == 1 ? error_code() : errc::make_error_code(errc::not_supported),
			readCount_ == 1 ? HEADER_LENGTH : RECEIVED_MESSAGE.size()
		); 
	}

	void AsyncWriteImplWithErrorParam(tcp::socket& socket, boost::asio::mutable_buffer& message,
		std::function<void(error_code error, size_t bytesTransferred)>&& handlerWrapper)
	{
		ValidateSentMessage(message);
		handlerWrapper(errc::make_error_code(errc::not_supported), 0); // place the handler in the executor without waiting
	}

	void AsyncConnectImplWithErrorParam(tcp::socket& socket, tcp::resolver::results_type endPoints,
		std::function<void(error_code error, tcp::endpoint endPoint)>&& handler)
	{
		ValidateEndPoint(endPoints);
		handler(errc::make_error_code(errc::not_supported), endPoints.begin()->endpoint());
	}

private:
	bool CalledReadTooManyTimes() { return readCount_++ > 1; }

	void ReceiveMessage(std::vector<char>& inMessage, const std::string& message)
	{
		for (size_t i = 0; i < message.size(); ++i)
			inMessage[i] = message[i];
	}

	void ValidateSentMessage(boost::asio::mutable_buffer& message)
	{
		size_t podSize = sizeof(PODType);
		std::vector<PODType> messageVector(message.size() / podSize);
		boost::asio::buffer_copy(boost::asio::buffer(messageVector), message);
		ASSERT_EQ(messageVector.size(), SENT_MESSAGE_WITH_HEADER.size());
		for (size_t i = 0; i < SENT_MESSAGE_WITH_HEADER.size(); ++i)
			EXPECT_EQ(messageVector[i], SENT_MESSAGE_WITH_HEADER.at(i));
	}

	void ValidateEndPoint(tcp::resolver::results_type& endPoint)
	{
		EXPECT_EQ(endPoint.begin()->endpoint().address(), IP_ADDRESS);
		EXPECT_EQ(endPoint.begin()->endpoint().port(), PORT);
	}

	size_t readCount_{ 0 };
};

struct MockConnHandler : public IConnectionHandler<PODType, IOAdapter>	
{
	MockConnHandler(io_context& context) : 
		IConnectionHandler(context) {}
	void StartApplication(IConnectionHandler::shared_conn_handler_t) override { ++startCount_; }

	static void ResetStartCount() { startCount_ = 0; }
	static size_t GetStartCount() { return startCount_; }

private:
	static size_t startCount_;
};

size_t MockConnHandler::startCount_{ 0 };
} // end namespace

TEST(IConnectionHandler, Construct)
{
	io_context context;
	EXPECT_NO_THROW(MockConnHandler handler(context));
}

TEST(IConnectionHandler, Socket)
{
	io_context context;
	MockConnHandler handler(context);

	tcp::socket& soket = handler.Socket();
	EXPECT_EQ(&(soket.get_io_context()), &context);
}

TEST(IConnectionHandler, IOService)
{
	io_context context;
	MockConnHandler handler(context);

	io_context& contextRef = handler.IOService();
	EXPECT_EQ(&(contextRef), &context);
}

TEST(IConnectionHandler, IOAdapter)
{
	io_context context;
	MockConnHandler handler(context);

	EXPECT_NO_THROW(handler.IOAdapter());
}

TEST(IConnectionHandler, PostReceiveMessages)
{
	io_context context;

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

	EXPECT_CALL(handler->IOAdapter(), AsyncRead(_, _, _, _)).
		Times(3).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadImpl)
			)
		);

	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
}

TEST(IConnectionHandler, PostReceiveMessagesWithErrorReadingHeader)
{
	io_context context;

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

	EXPECT_CALL(handler->IOAdapter(), AsyncRead(_, _, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadImplWithErrorParamReadingHeader)
			)
		);

	EXPECT_FALSE(handler->HasReceivedMessages());
	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
	EXPECT_FALSE(handler->HasReceivedMessages());
}

TEST(IConnectionHandler, PostReceiveMessagesWithErrorReadingHeaderLength)
{
	io_context context;

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

	EXPECT_CALL(handler->IOAdapter(), AsyncRead(_, _, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadImplWithErrorParamReadingHeaderLength)
			)
		);

	EXPECT_FALSE(handler->HasReceivedMessages());
	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
	EXPECT_FALSE(handler->HasReceivedMessages());
}

TEST(IConnectionHandler, PostReceiveMessagesWithErrorReadingBody)
{
	io_context context;

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

	EXPECT_CALL(handler->IOAdapter(), AsyncRead(_, _, _, _)).
		Times(2).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadImplWithErrorParamReadingBody)
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

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

	EXPECT_CALL(handler->IOAdapter(), AsyncRead(_, _, _, _)).
		Times(3).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadImpl)
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

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

	EXPECT_CALL(handler->IOAdapter(), AsyncRead(_, _, _, _)).
		Times(3).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncReadImpl)
			)
		);

	EXPECT_FALSE(handler->HasReceivedMessages());
	
	handler->PostReceiveMessages();
	EXPECT_EQ(context.run(), 1);
	ASSERT_TRUE(handler->HasReceivedMessages());

	std::vector<PODType> message;
	EXPECT_NO_THROW(message = handler->GetOneMessage());

	ASSERT_TRUE(message.size() == RECEIVED_MESSAGE.size());
	for (size_t i = 0; i < message.size(); ++i)
		EXPECT_EQ(message[i], RECEIVED_MESSAGE[i]);
}

TEST(IConnectionHandler, GetOneMessageThrows)
{
	io_context context;
	MockConnHandler handler(context);

	ASSERT_FALSE(handler.HasReceivedMessages());
	EXPECT_THROW(handler.GetOneMessage(), std::runtime_error);
}

TEST(IConnectionHandler, PostOutgoingMessage)
{
	io_context context;

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

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

TEST(IConnectionHandler, PostOutgoingMessages)
{
	io_context context;

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

	EXPECT_CALL(handler->IOAdapter(), AsyncWrite(_, _, _)).
		Times(2).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncWriteImpl)
			)
		);

	EXPECT_FALSE(handler->HasOutgoingMessages());

	std::vector<PODType> message(SENT_MESSAGE.cbegin(), SENT_MESSAGE.cend());
	EXPECT_NO_THROW(handler->PostOutgoingMessage(message));
	EXPECT_NO_THROW(handler->PostOutgoingMessage(message));
	EXPECT_TRUE(handler->HasOutgoingMessages());

	EXPECT_EQ(context.run(), 3);
	EXPECT_FALSE(handler->HasOutgoingMessages());
}

TEST(IConnectionHandler, PostOutgoingMessageWithError)
{
	io_context context;

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

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
	EXPECT_FALSE(handler->HasOutgoingMessages());
}

TEST(IConnectionHandler, Connect)
{
	io_context context;
	tcp::resolver resolver(context);
	tcp::resolver::results_type endPoints = resolver.resolve({ RAW_IP_ADDRESS, std::to_string(PORT) });

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

	EXPECT_CALL(handler->IOAdapter(), AsyncConnect(_, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncConnectImpl)
			)
		);

	MockConnHandler::ResetStartCount();
	EXPECT_NO_THROW(handler->Connect(endPoints));
	EXPECT_EQ(MockConnHandler::GetStartCount(), 1);
}

TEST(IConnectionHandler, ConnectWithError)
{
	io_context context;
	tcp::resolver resolver(context);
	tcp::resolver::results_type endPoints = resolver.resolve({ RAW_IP_ADDRESS, std::to_string(PORT) });

	// we use a shared_ptr here because MockConnHandler must be shared_from_this to execute
	auto handler = std::make_shared<MockConnHandler>(context);

	EXPECT_CALL(handler->IOAdapter(), AsyncConnect(_, _, _)).
		Times(1).
		WillRepeatedly(
			DoAll(
				Invoke(&(handler->IOAdapter()), &IOAdapter::AsyncConnectImplWithErrorParam)
			)
		);

	MockConnHandler::ResetStartCount();
	EXPECT_NO_THROW(handler->Connect(endPoints));
	EXPECT_EQ(MockConnHandler::GetStartCount(), 0);
}

TEST(IConnectionHandler, ConnectThrowsIfSocketIsOpen)
{
	io_context context;
	tcp::resolver resolver(context);
	tcp::resolver::results_type endPoints = resolver.resolve({ RAW_IP_ADDRESS, std::to_string(PORT) });

	MockConnHandler handler(context);
	handler.Socket().open(tcp::v4());
	EXPECT_THROW(handler.Connect(endPoints), std::runtime_error);
}

