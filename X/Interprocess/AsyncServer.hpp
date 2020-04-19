
#define TEMPLATE_T template<typename ConnHandlerT, typename AcceptorT>
#define AsyncServer_t AsyncServer<ConnHandlerT, AcceptorT>

TEMPLATE_T
AsyncServer_t::AsyncServer(boost::asio::io_context& context, const size_t numThreads) :
	ioService_(context),
	numThreads_(numThreads),
	acceptor_(context)
{
	if (numThreads_ == 0)
		throw std::runtime_error("Cannot construct AsyncServer because number of threads is invalid");
}

TEMPLATE_T
AsyncServer_t::~AsyncServer()
{
	Join();
}

TEMPLATE_T
void AsyncServer_t::Join()
{
	ioService_.stop();

	for (std::thread& th : threadPool_)
	{
		if (th.joinable())
			th.join();
	}
}

TEMPLATE_T
void AsyncServer_t::Start(const boost::asio::ip::tcp::endpoint& endPoint)
{
	using boost::asio::ip::tcp;
	using boost::system::error_code;

	error_code ec;

	acceptor_.open(endPoint.protocol(), ec);
	if (ec) throw std::runtime_error("AsyncServer could not open protocol via port " + std::to_string(endPoint.port()));

	acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
	if (ec) throw std::runtime_error("AsyncServer could not set option to reuse addresses");

	acceptor_.bind(endPoint, ec);
	if (ec) throw std::runtime_error("AsyncServer could not bind on endpoint");

	acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
	if (ec) throw std::runtime_error("AsyncServer could not listen");

	auto handler = std::make_shared<ConnHandlerT>(ioService_);
	acceptor_.async_accept(handler->Socket(), [=](error_code errorCode) 
	{
		HandleNewConnection(handler, errorCode);
	});

	for (size_t i = 0; i < numThreads_; ++i)
		threadPool_.emplace_back([=]() { ioService_.run(); });
}

TEMPLATE_T
void AsyncServer_t::HandleNewConnection(AsyncServer::shared_conn_handler_t handler, const boost::system::error_code errorCode)
{
	using boost::system::error_code;

	if (errorCode) return;

	handler->StartApplication(handler);

	auto newHandler = std::make_shared<ConnHandlerT>(ioService_);
	acceptor_.async_accept(newHandler->Socket(), [=](error_code ec)
	{
		HandleNewConnection(newHandler, ec);
	});
}

TEMPLATE_T
size_t AsyncServer_t::GetNumThreads() const
{
	return numThreads_;
}

TEMPLATE_T
AcceptorT& AsyncServer_t::GetAcceptor()
{
	return acceptor_;
}

#undef TEMPLATE_T 
#undef AsyncServer_t 

