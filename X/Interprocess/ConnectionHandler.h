#ifndef interprocess_connection_handler_h
#define interprocess_connection_handler_h

#include <deque>
#include <memory>

#include "config.h"

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

#include "Interprocess/AsyncIO.h"

namespace interprocess {

template<typename PacketT>
class ConnectionHandler : 
	public std::enable_shared_from_this<ConnectionHandler<PacketT>>
{
public:
	ConnectionHandler(boost::asio::io_context& ioService, AsyncIO<PacketT>& packetAsio);

	virtual ~ConnectionHandler();
	ConnectionHandler(const ConnectionHandler&) = delete;
	ConnectionHandler& operator=(const ConnectionHandler&) = delete;
	ConnectionHandler(ConnectionHandler&&) = delete;
	ConnectionHandler& operator=(ConnectionHandler&&) = delete;

	//incoming
	void Start();
	PacketT ReceivePacket();
	bool HasReceivedPacket() const;

	//outgoing
	void SendPacket(PacketT packet);

	boost::asio::ip::tcp::socket& Socket();
	boost::asio::io_context& IOService();
	AsyncIO<PacketT>& PacketAsyncIO();

protected:
	//incoming
	void QueueIncomingPacket(const boost::system::error_code& error, size_t bytesTransferred);
	void ReceivePacketStart();

	//outgoing
	void QueueOutgoingPacket(PacketT packet);
	void SendPacketStart();
	void SendPacketDone(const boost::system::error_code& error);

	// incoming
	const char UNTIL_CONDITION = '\0';
	boost::asio::streambuf inPacket_;
	std::deque<PacketT> inPacketQue_;

	//outgoing
	boost::asio::io_context::strand writeStrand_;
	boost::asio::io_context& ioServiceRef_;
	std::deque<PacketT> outPacketQue_;

	boost::asio::ip::tcp::socket socket_;
	AsyncIO<PacketT>& packetAsioRef_;
};

#include "ConnectionHandler.hpp"

} // end namespace interprocess
#endif // interprocess_connection_handler_h

