#ifndef interprocess_iconnection_handler_h
#define interprocess_iconnection_handler_h

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

template<typename PODType, typename AsioAdapterT = AsioAdapter<PODType>>
class IConnectionHandler : 
	public std::enable_shared_from_this<IConnectionHandler<PODType, AsioAdapterT>>
{
public:
	using shared_conn_handler_t = std::shared_ptr<IConnectionHandler>;

	IConnectionHandler(boost::asio::io_context& ioService);

	virtual ~IConnectionHandler();
	IConnectionHandler(const IConnectionHandler&) = delete;
	IConnectionHandler& operator=(const IConnectionHandler&) = delete;
	IConnectionHandler(IConnectionHandler&&) = delete;
	IConnectionHandler& operator=(IConnectionHandler&&) = delete;

	virtual void StartApplication(shared_conn_handler_t thisHandler);

	//incoming
	void PostReceiveMessages();
	bool HasReceivedMessages() const;

	std::vector<PODType> GetOneMessage();

	//outgoing
	void PostOutgoingMessage(std::vector<PODType> message);
	bool HasOutgoingMessages() const;

	//connection
	void Connect(boost::asio::ip::tcp::resolver::results_type endPoints);

	//close
	void Close();

	// accessors
	boost::asio::ip::tcp::socket& Socket();
	boost::asio::io_context& IOService();
	AsioAdapterT& IOAdapter();

private:
	//incoming
	void QueueReceivedMessage(size_t bytesTransferred);
	size_t ReceivedDataLength(const size_t bytesTransferreds);

	//outgoing
	std::vector<PODType> WrapWithHeader(const std::vector<PODType> message) const;
	void SendMessageStart();
	void SendMessageDone();

	// incoming
	const size_t HEADER_LENGTH = 4;
	std::vector<char> inMessage_;
	mutable std::mutex readLock_;
	boost::asio::io_context::strand readStrand_;
	std::deque<std::vector<PODType>> inMessageQue_;

	//outgoing
	mutable std::mutex writeLock_;
	boost::asio::io_context::strand writeStrand_;
	std::deque<std::vector<PODType>> outMessageQue_;

	boost::asio::io_context& ioServiceRef_;
	boost::asio::ip::tcp::socket socket_;
	AsioAdapterT ioAdapter_;
};

#include "IConnectionHandler.hpp"

} // end namespace interprocess
#endif // interprocess_iconnection_handler_h

