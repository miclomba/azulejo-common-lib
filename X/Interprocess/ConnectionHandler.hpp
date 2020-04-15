
#define TEMPLATE_T template<typename PODType>
#define ConnectionHandler_t ConnectionHandler<PODType>

TEMPLATE_T
ConnectionHandler_t::ConnectionHandler(boost::asio::io_context& ioService, AsioAdapter<PODType>& ioAdapter) :
	ioServiceRef_(ioService),
	ioAdapterRef_(ioAdapter),
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
AsioAdapter<PODType>& ConnectionHandler_t::IOAdapter()
{
	return ioAdapterRef_;
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
	ioAdapterRef_.AsyncReadUntil(Socket(), inMessage_, UNTIL_CONDITION,
		readStrand_.wrap([me = shared_from_this()](const boost::system::error_code& error, size_t bytesTransferred)
	{
		auto meDerived = static_cast<ConnectionHandler*>(me.get());
		meDerived->QueueReceivedMessage(error, bytesTransferred);
	}));
}

TEMPLATE_T
void ConnectionHandler_t::QueueReceivedMessage(const boost::system::error_code& error, size_t bytesTransferred)
{
	if (error) return;

	std::istream stream(&inMessage_);

	std::vector<PODType> message;

	// TODO
	// either write a stream operator or use the bytes transferred var to know how to build the message
	stream >> message;

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
	ioAdapterRef_.AsyncWrite(Socket(), boost::asio::buffer(outMessageQue_.front()),
		writeStrand_.wrap([me = shared_from_this()](const boost::system::error_code& error, size_t)
	{
		auto meDerived = static_cast<ConnectionHandler*>(me.get());
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

