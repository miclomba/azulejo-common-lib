
#define TEMPLATE_T template<typename PacketT>
#define ConnectionHandler_t ConnectionHandler<PacketT>

TEMPLATE_T
ConnectionHandler_t::ConnectionHandler(boost::asio::io_context& ioService, AsyncIO<PacketT>& packetAsio) :
	ioServiceRef_(ioService),
	packetAsioRef_(packetAsio),
	socket_(ioService),
	writeStrand_(ioService),
	readStrand_(ioService)
{
}

TEMPLATE_T
ConnectionHandler_t::~ConnectionHandler() = default;

TEMPLATE_T
boost::asio::ip::tcp::socket& ConnectionHandler_t::Socket()
{
	return socket_;
}

TEMPLATE_T
boost::asio::io_context& ConnectionHandler_t::IOService()
{
	return ioServiceRef_;
}

TEMPLATE_T
AsyncIO<PacketT>& ConnectionHandler_t::PacketAsyncIO()
{
	return packetAsioRef_;
}

TEMPLATE_T
void ConnectionHandler_t::PostReceiveMessages()
{
	ioServiceRef_.post(readStrand_.wrap(
		[me = shared_from_this()]()
	{
		auto meDerived = static_cast<ConnectionHandler*>(me.get());
		meDerived->ReceiveMessages();
	}));
}

TEMPLATE_T
void ConnectionHandler_t::ReceiveMessages()
{
	ReceiveMessageStart();
}

TEMPLATE_T
bool ConnectionHandler_t::HasOutgoingMessages() const
{
	return !outMessageQue_.empty();
}

TEMPLATE_T
void ConnectionHandler_t::PostOutgoingMessage(const std::vector<PacketT>& packets)
{
	// writing packets to a socket stream requires synchronization
	ioServiceRef_.post(writeStrand_.wrap(
		[me = shared_from_this(), packets]()
	{
		auto meDerived = static_cast<ConnectionHandler*>(me.get());
		meDerived->QueueOutgoingMessage(packets);
	}));
}

TEMPLATE_T
void ConnectionHandler_t::QueueOutgoingMessage(std::vector<PacketT> packets)
{
	bool writeInProgress = !outMessageQue_.empty();
	outMessageQue_.push_back(std::move(packets));

	if (!writeInProgress)
		SendMessageStart();
}

TEMPLATE_T
void ConnectionHandler_t::SendMessageStart()
{
	outMessageQue_.front() += UNTIL_CONDITION;
	packetAsioRef_.AsyncWrite(Socket(), boost::asio::buffer(outMessageQue_.front()),
		[me = shared_from_this()](const boost::system::error_code& error, size_t)
	{
		auto meDerived = static_cast<ConnectionHandler*>(me.get());
		meDerived->SendMessageDone(error);
	});
}

TEMPLATE_T
void ConnectionHandler_t::SendMessageDone(const boost::system::error_code& error)
{
	if (error) return;

	outMessageQue_.pop_front();
	if (!outMessageQue_.empty())
		SendMessageStart();
}

TEMPLATE_T
bool ConnectionHandler_t::HasReceivedMessages() const
{
	return !inMessageQue_.empty();
}

TEMPLATE_T
std::vector<PacketT> ConnectionHandler_t::GetOneMessage()
{
	if (inMessageQue_.empty())
		throw std::runtime_error("Cannot receive packet from ConnectionHandler because there are no packets to receive");

	std::vector<PacketT> packets = std::move(inMessageQue_.front());
	inMessageQue_.pop_front();

	return packets;
}

TEMPLATE_T
void ConnectionHandler_t::ReceiveMessageStart()
{
	// reading packets from a socket stream does not require synchronization 
	packetAsioRef_.AsyncReadUntil(Socket(), inMessage_, UNTIL_CONDITION,
		[me = shared_from_this()](const boost::system::error_code& error, size_t bytesTransferred)
	{
		auto meDerived = static_cast<ConnectionHandler*>(me.get());
		meDerived->QueueIncomingMessage(error, bytesTransferred);
	});
}

TEMPLATE_T
void ConnectionHandler_t::QueueIncomingMessage(const boost::system::error_code& error, size_t bytesTransferred)
{
	if (error) return;

	std::istream stream(&inMessage_);

	std::vector<PacketT> packet;

	// TODO
	// either write a stream operator or use the bytes transferred var to know how to build the packet
	stream >> packet;

	inMessageQue_.push_back(std::move(packet));

	ReceiveMessageStart();
}

#undef TEMPLATE_T 
#undef ConnectionHandler_t 

