#ifndef interprocess_connection_handler_h
#define interprocess_connection_handler_h

#include <deque>
#include <memory>
#include <mutex>
#include <vector>

#include "config.h"

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

#include "Interprocess/AsioAdapter.h"

namespace interprocess {

template<
	typename PODType, 
	typename AsioAdapterT = AsioAdapter<PODType>, 
	typename SocketT = boost::asio::ip::tcp::socket
>
class ConnectionHandler : 
	public std::enable_shared_from_this<ConnectionHandler<PODType, AsioAdapterT, SocketT>>
{
public:
	ConnectionHandler(
		boost::asio::io_context& ioService,
		const boost::asio::ip::tcp::endpoint& endPoint
	);

	virtual ~ConnectionHandler();
	ConnectionHandler(const ConnectionHandler&) = delete;
	ConnectionHandler& operator=(const ConnectionHandler&) = delete;
	ConnectionHandler(ConnectionHandler&&) = delete;
	ConnectionHandler& operator=(ConnectionHandler&&) = delete;

	virtual void StartApplication() = 0;

	//incoming
	void PostReceiveMessages();
	bool HasReceivedMessages() const;

	std::vector<PODType> GetOneMessage();

	//outgoing
	void PostOutgoingMessage(std::vector<PODType> message);
	bool HasOutgoingMessages() const;

	SocketT& Socket();
	boost::asio::io_context& IOService();
	AsioAdapterT& IOAdapter();

private:
	//incoming
	void QueueReceivedMessage(const boost::system::error_code& error, size_t bytesTransferred);

	//outgoing
	void SendMessageStart();
	void SendMessageDone(const boost::system::error_code& error);

	// incoming
	const char UNTIL_CONDITION = '\0';
	boost::asio::streambuf inMessage_;
	mutable std::mutex readLock_;
	boost::asio::io_context::strand readStrand_;
	std::deque<std::vector<PODType>> inMessageQue_;

	//outgoing
	mutable std::mutex writeLock_;
	boost::asio::io_context::strand writeStrand_;
	std::deque<std::vector<PODType>> outMessageQue_;

	boost::asio::io_context& ioServiceRef_;
	std::shared_ptr<SocketT> socket_;
	std::shared_ptr<AsioAdapterT> ioAdapter_;
};

#include "ConnectionHandler.hpp"

} // end namespace interprocess
#endif // interprocess_connection_handler_h

