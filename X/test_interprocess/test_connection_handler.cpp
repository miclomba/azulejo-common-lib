#include "config.h"

#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Interprocess/AsyncIO.h"
#include "Interprocess/ConnectionHandler.h"

using boost::asio::io_context;
using boost::asio::ip::tcp;
using boost::asio::streambuf;
using boost::system::error_code;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

using interprocess::AsyncIO;
using interprocess::ConnectionHandler;

namespace
{
struct Packet {
	friend void operator>>(std::istream& stream, std::vector<Packet>& packet);
};

void operator>>(std::istream& stream, std::vector<Packet>& packet)
{
}

struct PacketIO : public AsyncIO<Packet>
{
	MOCK_METHOD4(AsyncReadUntil, void(tcp::socket&, streambuf&, const char,
		std::function<void(const error_code& error, size_t bytesTransferred)>)
	);

	MOCK_METHOD3(AsyncWrite, void(tcp::socket&, std::vector<Packet>& packet,
		std::function<void(const error_code& error, size_t bytesTransferred)>)
	);

	void AsyncReadUntilImpl(tcp::socket& socket, streambuf& inPacket, const char UNTIL_CONDITION,
		std::function<void(const error_code& error, size_t bytesTransferred)>&& handlerWrapper)
	{
		if (CalledReadMoreThanOnce()) return;

		// place the handler in the executor without waiting
		handlerWrapper(error_code(), 0);
	}

	void AsyncWriteImpl(tcp::socket& socket, std::vector<Packet>& packet,
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

struct MockConnHandler : public ConnectionHandler<Packet> {
	MockConnHandler(io_context& context, PacketIO& packetAsio) : ConnectionHandler(context, packetAsio) {}
};
} // end namespace

TEST(ConnectionHandler, Construct)
{
	io_context context;
	PacketIO packetAsio;
	EXPECT_NO_THROW(MockConnHandler handler(context, packetAsio));
}

TEST(ConnectionHandler, Socket)
{
	io_context context;
	PacketIO packetAsio;
	MockConnHandler handler(context, packetAsio);

	tcp::socket& soket = handler.Socket();
	EXPECT_EQ(&(soket.get_io_context()), &context);
}

TEST(ConnectionHandler, IOService)
{
	io_context context;
	PacketIO packetAsio;
	MockConnHandler handler(context, packetAsio);

	io_context& contextRef = handler.IOService();
	EXPECT_EQ(&(contextRef), &context);
}

TEST(ConnectionHandler, PacketAsyncIO)
{
	io_context context;
	auto packetAsio = std::make_shared<AsyncIO<Packet>>();
	auto handler = std::make_shared<ConnectionHandler<Packet>>(context, *packetAsio);

	EXPECT_EQ(&(handler->PacketAsyncIO()),packetAsio.get());
}

TEST(ConnectionHandler, PostReceiveMessages)
{
	io_context context;
	auto packetAsio = std::make_shared<PacketIO>();
	auto handler = std::make_shared<MockConnHandler>(context, *packetAsio);

	EXPECT_CALL(*packetAsio, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(packetAsio.get(), &PacketIO::AsyncReadUntilImpl));

	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);

	// a second read indicates we completed one read cycle before forcing it to quit
	EXPECT_EQ(packetAsio->GetReadCount(), 2);
}

TEST(ConnectionHandler, HasReceivedMessages)
{
	io_context context;
	auto packetAsio = std::make_shared<PacketIO>();
	auto handler = std::make_shared<MockConnHandler>(context, *packetAsio);

	EXPECT_CALL(*packetAsio, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(packetAsio.get(), &PacketIO::AsyncReadUntilImpl));

	EXPECT_FALSE(handler->HasReceivedMessages());
	EXPECT_NO_THROW(handler->PostReceiveMessages());
	EXPECT_EQ(context.run(), 1);
	EXPECT_TRUE(handler->HasReceivedMessages());
}

TEST(ConnectionHandler, GetPacket)
{
	io_context context;
	auto packetAsio = std::make_shared<PacketIO>();
	auto handler = std::make_shared<MockConnHandler>(context, *packetAsio);

	EXPECT_CALL(*packetAsio, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(packetAsio.get(), &PacketIO::AsyncReadUntilImpl));

	EXPECT_FALSE(handler->HasReceivedMessages());
	
	handler->PostReceiveMessages();
	EXPECT_EQ(context.run(), 1);
	ASSERT_TRUE(handler->HasReceivedMessages());

	std::vector<Packet> packets;
	EXPECT_NO_THROW(packets = handler->GetOneMessage());
}

TEST(ConnectionHandler, GetPacketThrows)
{
	io_context context;
	auto packetAsio = std::make_shared<PacketIO>();
	auto handler = std::make_shared<MockConnHandler>(context, *packetAsio);

	EXPECT_CALL(*packetAsio, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(packetAsio.get(), &PacketIO::AsyncReadUntilImpl));

	ASSERT_FALSE(handler->HasReceivedMessages());
	EXPECT_THROW(handler->GetOneMessage(), std::runtime_error);
}

TEST(ConnectionHandler, PostOutgoingMessage)
{
	io_context context;
	auto packetAsio = std::make_shared<PacketIO>();
	auto handler = std::make_shared<MockConnHandler>(context, *packetAsio);

	//EXPECT_CALL(*packetAsio, AsyncWrite(_, _, _)).WillRepeatedly(Invoke(packetAsio.get(), &PacketIO::AsyncWriteImpl));

	//EXPECT_FALSE(handler->HasOutgoingMessages());
	//EXPECT_NO_THROW(handler->PostOutgoingMessage(Packet()));
	//EXPECT_TRUE(handler->HasOutgoingMessages());

	//context.run();
	//EXPECT_FALSE(handler->HasOutgoingMessages());
}
