
#define TEMPLATE_T template<typename ConnHandlerT, typename AcceptorT>
#define TCPServer_t TCPServer<ConnHandlerT, AcceptorT>

TEMPLATE_T
TCPServer_t::TCPServer(boost::asio::io_context& context, const size_t numThreads) :
	ioService_(context),
	numThreads_(numThreads),
	acceptor_(context)
{
	if (numThreads_ == 0)
		throw std::runtime_error("Cannot construct TCPServer because number of threads is invalid");
}

TEMPLATE_T
TCPServer_t::~TCPServer()
{
	Join();
}

TEMPLATE_T
void TCPServer_t::Join()
{
	ioService_.stop();

	for (std::thread& th : threadPool_)
	{
		if (th.joinable())
			th.join();
	}
}

TEMPLATE_T
void TCPServer_t::Start(const boost::asio::ip::tcp::endpoint& endPoint)
{
	using boost::asio::ip::tcp;
	using boost::system::error_code;

	error_code ec;

	acceptor_.open(endPoint.protocol(), ec);
	if (ec) throw std::runtime_error("TCPServer could not accept on port " + std::to_string(endPoint.port()));

	acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
	if (ec) throw std::runtime_error("TCPServer could not set option to reuse addresses");

	acceptor_.bind(endPoint, ec);
	if (ec) throw std::runtime_error("TCPServer could not bind on endpoint");

	acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
	if (ec) throw std::runtime_error("TCPServer could not listen");

	auto handler = std::make_shared<ConnHandlerT>(ioService_);
	acceptor_.async_accept(handler->Socket(), [=](error_code errorCode) 
	{
		HandleNewConnection(handler, errorCode);
	});

	for (size_t i = 0; i < numThreads_; ++i)
		threadPool_.emplace_back([=]() { ioService_.run(); });
}

TEMPLATE_T
void TCPServer_t::HandleNewConnection(TCPServer::shared_conn_handler_t handler, const boost::system::error_code errorCode)
{
	using boost::system::error_code;

	if (errorCode) return;

	// TODO this needs to be put on the io service queue so that a new accept call can be made
	handler->StartApplication(handler);

	auto newHandler = std::make_shared<ConnHandlerT>(ioService_);
	acceptor_.async_accept(newHandler->Socket(), [=](error_code ec)
	{
		HandleNewConnection(newHandler, ec);
	});
}

TEMPLATE_T
size_t TCPServer_t::GetNumThreads() const
{
	return numThreads_;
}

TEMPLATE_T
AcceptorT& TCPServer_t::GetAcceptor()
{
	return acceptor_;
}

#undef TEMPLATE_T 
#undef TCPServer_t 

