
#define TEMPLATE_T template<typename ConnHandlerT, typename ConnAcceptorT>
#define AsyncServer_t AsyncServer<ConnHandlerT, ConnAcceptorT>

TEMPLATE_T
AsyncServer_t::AsyncServer(std::shared_ptr<ConnAcceptorT> acceptor, const size_t numThreads) :
	numThreads_(numThreads),
	acceptor_(std::move(acceptor))
{
	if (!acceptor_)
		throw std::runtime_error("Cannot construct AsyncServer because acceptor is invalid");
	if (numThreads_ == 0)
		throw std::runtime_error("Cannot construct AsyncServer because number of threads is invalid");

	ioService_ = &(acceptor_->get_io_context());
}

TEMPLATE_T
AsyncServer_t::~AsyncServer()
{
	Join();
}

TEMPLATE_T
void AsyncServer_t::Join()
{
	ioService_->stop();

	for (std::thread& th : threadPool_)
	{
		if (th.joinable())
			th.join();
	}
}

TEMPLATE_T
void AsyncServer_t::Start(const boost::asio::ip::tcp::endpoint& endPoint)
{
	assert(ioService_);

	using boost::asio::ip::tcp;
	using boost::system::error_code;

	error_code ec;

	endPoint_ = endPoint;
	acceptor_->open(endPoint_.protocol(), ec);
	if (ec) throw std::runtime_error("AsyncServer could not open protocol via port " + std::to_string(endPoint_.port()));

	acceptor_->set_option(tcp::acceptor::reuse_address(true), ec);
	if (ec) throw std::runtime_error("AsyncServer could not set option to reuse addresses");

	acceptor_->bind(endPoint_, ec);
	if (ec) throw std::runtime_error("AsyncServer could not bind on endpoint");

	acceptor_->listen(boost::asio::socket_base::max_listen_connections, ec);
	if (ec) throw std::runtime_error("AsyncServer could not listen");

	auto handler = std::make_shared<ConnHandlerT>(*ioService_, endPoint_);
	acceptor_->async_accept(handler->Socket(), [=](error_code errorCode) 
	{
		HandleNewConnection(handler, errorCode);
	});

	for (size_t i = 0; i < numThreads_; ++i)
		threadPool_.emplace_back([=]() { ioService_->run(); });
}

TEMPLATE_T
void AsyncServer_t::HandleNewConnection(AsyncServer::shared_conn_handler_t handler, const boost::system::error_code errorCode)
{
	using boost::system::error_code;

	if (errorCode) return;

	handler->Start();

	auto newHandler = std::make_shared<ConnHandlerT>(*ioService_, endPoint_);
	acceptor_->async_accept(newHandler->Socket(), [=](error_code ec)
	{
		HandleNewConnection(newHandler, ec);
	});
}

TEMPLATE_T
size_t AsyncServer_t::GetNumThreads() const
{
	return numThreads_;
}

#undef TEMPLATE_T 
#undef AsyncServer_t 

