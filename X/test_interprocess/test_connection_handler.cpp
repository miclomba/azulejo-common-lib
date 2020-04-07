#include "config.h"

#include <string>

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
struct Packet : std::string {
	friend void operator>>(std::istream& stream, Packet& packet);
};

void operator>>(std::istream& stream, Packet& packet)
{
}

struct PacketIO : public AsyncIO<Packet>
{
	MOCK_METHOD4(AsyncReadUntil, void(tcp::socket&, streambuf&, const char,
		std::function<void(const error_code& error, size_t bytesTransferred)>)
	);

	void AsyncReadUntilImpl(tcp::socket& socket, streambuf& inPacket, const char UNTIL_CONDITION,
		std::function<void(const error_code& error, size_t bytesTransferred)> handler)
	{
		if (readCount_ > 0)
			return;

		++readCount_;
		handler(error_code(), 0);
	}

	size_t GetReadCount() const
	{
		return readCount_;
	}

private:
	size_t readCount_{ 0 };
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

TEST(ConnectionHandler, Start)
{
	io_context context;
	auto packetAsio = std::make_shared<AsyncIO<Packet>>();
	auto handler = std::make_shared<ConnectionHandler<Packet>>(context, *packetAsio);

	EXPECT_NO_THROW(handler->Start());
}

TEST(ConnectionHandler, HasReceivedPacket)
{
	io_context context;
	auto packetAsio = std::make_shared<PacketIO>();
	auto handler = std::make_shared<MockConnHandler>(context, *packetAsio);

	EXPECT_CALL(*packetAsio, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(packetAsio.get(), &PacketIO::AsyncReadUntilImpl));

	EXPECT_FALSE(handler->HasReceivedPacket());
	EXPECT_NO_THROW(handler->Start());
	EXPECT_TRUE(handler->HasReceivedPacket());
}

TEST(ConnectionHandler, ReceivePacket)
{
	io_context context;
	auto packetAsio = std::make_shared<PacketIO>();
	auto handler = std::make_shared<MockConnHandler>(context, *packetAsio);

	EXPECT_CALL(*packetAsio, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(packetAsio.get(), &PacketIO::AsyncReadUntilImpl));

	EXPECT_FALSE(handler->HasReceivedPacket());
	
	handler->Start();
	ASSERT_TRUE(handler->HasReceivedPacket());

	Packet packet;
	EXPECT_NO_THROW(packet = handler->ReceivePacket());
}

TEST(ConnectionHandler, ReceivePacketThrows)
{
	io_context context;
	auto packetAsio = std::make_shared<PacketIO>();
	auto handler = std::make_shared<MockConnHandler>(context, *packetAsio);

	EXPECT_CALL(*packetAsio, AsyncReadUntil(_, _, _, _)).WillRepeatedly(Invoke(packetAsio.get(), &PacketIO::AsyncReadUntilImpl));

	ASSERT_FALSE(handler->HasReceivedPacket());
	EXPECT_THROW(handler->ReceivePacket(), std::runtime_error);
}
