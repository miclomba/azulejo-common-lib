
#define TEMPLATE_T template<typename PODType, typename AsioAdapterT>
#define ConnectionHandler_t ConnectionHandler<PODType, AsioAdapterT>

TEMPLATE_T
ConnectionHandler_t::ConnectionHandler(
	boost::asio::io_context& ioService,
	const boost::asio::ip::tcp::endpoint& endPoint
) :
	ioServiceRef_(ioService),
	socket_(ioService),
	writeStrand_(ioService),
	readStrand_(ioService),
	ioAdapter_(std::make_shared<AsioAdapterT>())
{
	//socket_.open(endPoint.protocol());
	//socket_.bind(endPoint);
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
AsioAdapterT& ConnectionHandler_t::IOAdapter()
{
	assert(ioAdapter_);
	return *ioAdapter_;
}

TEMPLATE_T
bool ConnectionHandler_t::HasReceivedMessages() const
{
	const std::lock_guard<std::mutex> lock(readLock_);
	return !inMessageQue_.empty();
}

TEMPLATE_T
void ConnectionHandler_t::PostReceiveMessages()
{
	ioAdapter_->AsyncReadUntil(Socket(), inMessage_, UNTIL_CONDITION,
		readStrand_.wrap([me = shared_from_this()](const boost::system::error_code& error, size_t bytesTransferred)
	{
		auto meDerived = static_cast<ConnectionHandler_t*>(me.get());
		meDerived->QueueReceivedMessage(error, bytesTransferred);
	}));
}

TEMPLATE_T
void ConnectionHandler_t::QueueReceivedMessage(const boost::system::error_code& error, size_t bytesTransferred)
{
	if (error) return;

	size_t podSize = sizeof(PODType);
	std::vector<PODType> message(inMessage_.size() / podSize);
	boost::asio::buffer_copy(boost::asio::buffer(message), inMessage_.data());

	const std::lock_guard<std::mutex> lock(readLock_);
	inMessageQue_.push_back(std::move(message));

	PostReceiveMessages();
}

TEMPLATE_T
std::vector<PODType> ConnectionHandler_t::GetOneMessage()
{
	const std::lock_guard<std::mutex> lock(readLock_);

	if (inMessageQue_.empty())
		throw std::runtime_error("Cannot receive message from ConnectionHandler because there is no message to receive");

	std::vector<PODType> message = std::move(inMessageQue_.front());
	inMessageQue_.pop_front();

	return message;
}

TEMPLATE_T
bool ConnectionHandler_t::HasOutgoingMessages() const
{
	const std::lock_guard<std::mutex> lock(writeLock_);
	return !outMessageQue_.empty();
}

TEMPLATE_T
void ConnectionHandler_t::PostOutgoingMessage(const std::vector<PODType> message)
{
	{
		const std::lock_guard<std::mutex> lock(writeLock_);
		outMessageQue_.push_back(std::move(message));
	}

	SendMessageStart();
}

TEMPLATE_T
void ConnectionHandler_t::SendMessageStart()
{
	const std::lock_guard<std::mutex> lock(writeLock_);

	outMessageQue_.front().push_back(PODType('\0'));
	ioAdapter_->AsyncWrite(Socket(), boost::asio::buffer(outMessageQue_.front()),
		writeStrand_.wrap([me = shared_from_this()](const boost::system::error_code& error, size_t)
	{
		auto meDerived = static_cast<ConnectionHandler_t*>(me.get());
		meDerived->SendMessageDone(error);
	}));
}

TEMPLATE_T
void ConnectionHandler_t::SendMessageDone(const boost::system::error_code& error)
{
	if (error) return;

	const std::lock_guard<std::mutex> lock(writeLock_);

	outMessageQue_.pop_front();
	if (!outMessageQue_.empty())
		SendMessageStart();
}

#undef TEMPLATE_T 
#undef ConnectionHandler_t 

