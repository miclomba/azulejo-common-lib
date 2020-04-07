
#define TEMPLATE_T template<typename PacketT>
#define ConnectionHandler_t ConnectionHandler<PacketT>

TEMPLATE_T
ConnectionHandler_t::ConnectionHandler(boost::asio::io_context& ioService, AsyncIO<PacketT>& packetAsio) :
	ioServiceRef_(ioService),
	packetAsioRef_(packetAsio),
	socket_(ioService),
	writeStrand_(ioService)
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
void ConnectionHandler_t::Start()
{
	ReceivePacketStart();
}

TEMPLATE_T
void ConnectionHandler_t::SendPacket(PacketT packet)
{
	// writing packets to a socket stream requires synchronization
	ioServiceRef_.post(writeStrand_.wrap(
		[me = shared_from_this(), packet]()
	{
		auto meDerived = static_cast<ConnectionHandler*>(me.get());
		meDerived->QueueOutgoingPacket(packet);
	}));
}

TEMPLATE_T
void ConnectionHandler_t::QueueOutgoingPacket(PacketT packet)
{
	bool writeInProgress = !outPacketQue_.empty();
	outPacketQue_.push_back(std::move(packet));

	if (!writeInProgress)
		SendPacketStart();
}

TEMPLATE_T
void ConnectionHandler_t::SendPacketStart()
{
	outPacketQue_.front() += UNTIL_CONDITION;
	packetAsioRef_.AsyncWrite(Socket(), boost::asio::buffer(outPacketQue_.front()),
		[me = shared_from_this()](const boost::system::error_code& error, size_t)
	{
		auto meDerived = static_cast<ConnectionHandler*>(me.get());
		meDerived->SendPacketDone(error);
	});
}

TEMPLATE_T
void ConnectionHandler_t::SendPacketDone(const boost::system::error_code& error)
{
	if (error) return;

	outPacketQue_.pop_front();
	if (!outPacketQue_.empty())
		SendPacketStart();
}

TEMPLATE_T
bool ConnectionHandler_t::HasReceivedPacket() const
{
	return !inPacketQue_.empty();
}

TEMPLATE_T
PacketT ConnectionHandler_t::ReceivePacket()
{
	if (inPacketQue_.empty())
		throw std::runtime_error("Cannot receive packet from ConnectionHandler because there are no packets to receive");

	PacketT packet = std::move(inPacketQue_.front());
	inPacketQue_.pop_front();

	return packet;
}

TEMPLATE_T
void ConnectionHandler_t::ReceivePacketStart()
{
	// reading packets from a socket stream does not require synchronization 
	packetAsioRef_.AsyncReadUntil(Socket(), inPacket_, UNTIL_CONDITION,
		[me = shared_from_this()](const boost::system::error_code& error, size_t bytesTransferred)
	{
		auto meDerived = static_cast<ConnectionHandler*>(me.get());
		meDerived->QueueIncomingPacket(error, bytesTransferred);
	});
}

TEMPLATE_T
void ConnectionHandler_t::QueueIncomingPacket(const boost::system::error_code& error, size_t bytesTransferred)
{
	if (error) return;

	std::istream stream(&inPacket_);

	PacketT packet;

	// TODO
	// either write a stream operator or use the bytes transferred var to know how to build the packet
	stream >> packet;

	inPacketQue_.push_back(std::move(packet));

	ReceivePacketStart();
}

#undef TEMPLATE_T 
#undef ConnectionHandler_t 

