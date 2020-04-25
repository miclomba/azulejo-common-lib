
#define TEMPLATE_T template<typename PODType, typename AsioAdapterT>
#define IConnectionHandler_t IConnectionHandler<PODType, AsioAdapterT>

TEMPLATE_T
IConnectionHandler_t::IConnectionHandler(boost::asio::io_context& ioService) :
	ioServiceRef_(ioService),
	writeStrand_(ioService),
	readStrand_(ioService),
	socket_(ioService)
{
	const size_t BUFFER_SIZE = 512;
	inMessage_.resize(BUFFER_SIZE / sizeof(PODType), 0);
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
size_t IConnectionHandler_t::ReceivedDataLength(const size_t bytesTransferred)
{
	if (bytesTransferred != HEADER_LENGTH)
		throw std::runtime_error("IConnectionHandler cannot determine length of received data");

	std::vector<char> header(bytesTransferred + 1);
	std::memcpy(header.data(), inMessage_.data(), HEADER_LENGTH);
	header[HEADER_LENGTH] = '\0';
	return static_cast<size_t>(std::atoll(header.data()));
}

TEMPLATE_T
void IConnectionHandler_t::PostReceiveMessages()
{
	ioAdapter_.AsyncRead(Socket(), inMessage_, HEADER_LENGTH,
		readStrand_.wrap([meShared = shared_from_this()](boost::system::error_code error, size_t bytesTransferred)
	{
		if (error) return;

		auto me = static_cast<IConnectionHandler_t*>(meShared.get());
		size_t dataLength = me->ReceivedDataLength(bytesTransferred);

		if (dataLength == 0)
			return;

		me->ioAdapter_.AsyncRead(me->Socket(), me->inMessage_, dataLength,
			me->readStrand_.wrap([meShared](boost::system::error_code error, size_t bytesTransferred)
		{
			if (error) return;

			auto me = static_cast<IConnectionHandler_t*>(meShared.get());
			me->QueueReceivedMessage(bytesTransferred);
		}));
	}));
}

TEMPLATE_T
void IConnectionHandler_t::QueueReceivedMessage(size_t bytesTransferred)
{
	size_t podSize = sizeof(PODType);
	std::vector<PODType> message(bytesTransferred / podSize);
	std::memcpy(message.data(), inMessage_.data(), bytesTransferred);

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
std::vector<PODType> IConnectionHandler_t::WrapWithHeader(const std::vector<PODType> message) const
{
	std::string size = std::to_string(message.size());
	size_t spaces = HEADER_LENGTH - size.length();

	std::string header;
	for (size_t i = 0; i < spaces; ++i)
		header.push_back(' ');
	header += size;
	std::vector<PODType> headerAndMessage(header.begin(), header.end());
	headerAndMessage.insert(headerAndMessage.end(), message.begin(), message.end());

	return headerAndMessage;
}

TEMPLATE_T
void IConnectionHandler_t::PostOutgoingMessage(const std::vector<PODType> message)
{
	{
		std::vector<PODType> wrappedMessage = WrapWithHeader(std::move(message));

		const std::lock_guard<std::mutex> lock(writeLock_);
		outMessageQue_.push_back(std::move(wrappedMessage));
	}

	IOService().post([me = shared_from_this()]() 
	{
		auto meDerived = static_cast<IConnectionHandler_t*>(me.get());
		meDerived->SendMessageStart();
	});
}

TEMPLATE_T
void IConnectionHandler_t::SendMessageStart()
{
	auto message = std::make_shared<std::vector<PODType>>();
	{
		const std::lock_guard<std::mutex> lock(writeLock_);

		if (outMessageQue_.empty())
			return;

		*message = std::move(outMessageQue_.front());
		outMessageQue_.pop_front();
	}

	ioAdapter_.AsyncWrite(Socket(), boost::asio::buffer(*message),
		writeStrand_.wrap([me = shared_from_this(), message](boost::system::error_code error, size_t)
	{
		if (error) return;

		auto meDerived = static_cast<IConnectionHandler_t*>(me.get());
		meDerived->SendMessageDone();
	}));
}

TEMPLATE_T
void IConnectionHandler_t::SendMessageDone()
{
	const std::lock_guard<std::mutex> lock(writeLock_);

	if (!outMessageQue_.empty())
	{
		IOService().post([me = shared_from_this()]() 
		{
			auto meDerived = static_cast<IConnectionHandler_t*>(me.get());
			meDerived->SendMessageStart();
		});
	}
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

TEMPLATE_T
void IConnectionHandler_t::Close()
{
	IOService().post([me = shared_from_this()]() 
	{ 
		auto meDerived = static_cast<IConnectionHandler_t*>(me.get());
		meDerived->Socket().close(); 
	});
}

#undef TEMPLATE_T 
#undef IConnectionHandler_t 

