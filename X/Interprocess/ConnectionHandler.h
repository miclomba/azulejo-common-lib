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

template<typename PODType>
class ConnectionHandler : 
	public std::enable_shared_from_this<ConnectionHandler<PODType>>
{
public:
	ConnectionHandler(boost::asio::io_context& ioService, AsioAdapter<PODType>& ioAdapter);

	virtual ~ConnectionHandler();
	ConnectionHandler(const ConnectionHandler&) = delete;
	ConnectionHandler& operator=(const ConnectionHandler&) = delete;
	ConnectionHandler(ConnectionHandler&&) = delete;
	ConnectionHandler& operator=(ConnectionHandler&&) = delete;

	//incoming
	void PostReceiveMessages();
	bool HasReceivedMessages() const;

	std::vector<PODType> GetOneMessage();

	//outgoing
	void PostOutgoingMessage(std::vector<PODType> message);
	bool HasOutgoingMessages() const;

	boost::asio::ip::tcp::socket& Socket();
	boost::asio::io_context& IOService();
	AsioAdapter<PODType>& IOAdapter();

protected:
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
	boost::asio::ip::tcp::socket socket_;
	AsioAdapter<PODType>& ioAdapterRef_;
};

#include "ConnectionHandler.hpp"

} // end namespace interprocess
#endif // interprocess_connection_handler_h

