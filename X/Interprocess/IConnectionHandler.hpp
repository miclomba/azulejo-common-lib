
#define TEMPLATE_T template<typename PODType, typename AsioAdapterT>
#define IConnectionHandler_t IConnectionHandler<PODType, AsioAdapterT>

TEMPLATE_T
IConnectionHandler_t::IConnectionHandler(boost::asio::io_context& ioService) :
	ioServiceRef_(ioService),
	writeStrand_(ioService),
	readStrand_(ioService),
	socket_(ioService)
{
}

TEMPLATE_T
IConnectionHandler_t::~IConnectionHandler() = default;

TEMPLATE_T
boost::asio::ip::tcp::socket& IConnectionHandler_t::Socket()
{
	return socket_;
}

TEMPLATE_T
boost::asio::io_context& IConnectionHandler_t::IOService()
{
	return ioServiceRef_;
}

TEMPLATE_T
AsioAdapterT& IConnectionHandler_t::IOAdapter()
{
	return ioAdapter_;
}

TEMPLATE_T
bool IConnectionHandler_t::HasReceivedMessages() const
{
	const std::lock_guard<std::mutex> lock(readLock_);
	return !inMessageQue_.empty();
}

TEMPLATE_T
void IConnectionHandler_t::PostReceiveMessages()
{
	ioAdapter_.AsyncReadUntil(Socket(), inMessage_, UNTIL_CONDITION,
		readStrand_.wrap([me = shared_from_this()](boost::system::error_code error, size_t bytesTransferred)
	{
		auto meDerived = static_cast<IConnectionHandler_t*>(me.get());
		meDerived->QueueReceivedMessage(error, bytesTransferred);
	}));
}

TEMPLATE_T
void IConnectionHandler_t::QueueReceivedMessage(const boost::system::error_code& error, size_t bytesTransferred)
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
std::vector<PODType> IConnectionHandler_t::GetOneMessage()
{
	const std::lock_guard<std::mutex> lock(readLock_);

	if (inMessageQue_.empty())
		throw std::runtime_error("Cannot receive message from IConnectionHandler because there is no message to receive");

	std::vector<PODType> message = std::move(inMessageQue_.front());
	inMessageQue_.pop_front();

	return message;
}

TEMPLATE_T
bool IConnectionHandler_t::HasOutgoingMessages() const
{
	const std::lock_guard<std::mutex> lock(writeLock_);
	return !outMessageQue_.empty();
}

TEMPLATE_T
void IConnectionHandler_t::PostOutgoingMessage(const std::vector<PODType> message)
{
	{
		const std::lock_guard<std::mutex> lock(writeLock_);
		outMessageQue_.push_back(std::move(message));
	}

	SendMessageStart();
}

TEMPLATE_T
void IConnectionHandler_t::SendMessageStart()
{
	const std::lock_guard<std::mutex> lock(writeLock_);

	outMessageQue_.front().push_back(PODType('\0'));
	ioAdapter_.AsyncWrite(Socket(), boost::asio::buffer(outMessageQue_.front()),
		writeStrand_.wrap([me = shared_from_this()](boost::system::error_code error, size_t)
	{
		auto meDerived = static_cast<IConnectionHandler_t*>(me.get());
		meDerived->SendMessageDone(error);
	}));
}

TEMPLATE_T
void IConnectionHandler_t::SendMessageDone(const boost::system::error_code& error)
{
	if (error) return;

	const std::lock_guard<std::mutex> lock(writeLock_);

	outMessageQue_.pop_front();
	if (!outMessageQue_.empty())
		SendMessageStart();
}

TEMPLATE_T
void IConnectionHandler_t::Connect(boost::asio::ip::tcp::resolver::results_type endPoints)
{
	if (Socket().is_open())
		throw std::runtime_error("IConnectionHandler cannot connect because socket is already open");

	ioAdapter_.AsyncConnect(Socket(), endPoints,
		[me = shared_from_this()](boost::system::error_code error, boost::asio::ip::tcp::endpoint)
	{
		if (error) return;

		auto meDerived = static_cast<IConnectionHandler_t*>(me.get());
		meDerived->StartApplication(me);
	});
}

#undef TEMPLATE_T 
#undef IConnectionHandler_t 

