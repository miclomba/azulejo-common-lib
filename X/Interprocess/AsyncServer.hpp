
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
void AsyncServer_t::Start(const uint16_t port)
{
	using boost::asio::ip::tcp;

	tcp::endpoint endpoint(tcp::v4(), port);
	acceptor_->open(endpoint.protocol());
	acceptor_->set_option(tcp::acceptor::reuse_address(true));
	acceptor_->bind(endpoint);
	acceptor_->listen();

	auto handler = std::make_shared<ConnHandlerT>(*ioService_);
	acceptor_->async_accept(handler->Socket(), [=](auto ec) 
	{
		HandleNewConnection(handler, ec);
	});

	for (size_t i = 0; i < numThreads_; ++i)
		threadPool_.emplace_back([=]() { ioService_->run(); });
}

TEMPLATE_T
void AsyncServer_t::HandleNewConnection(AsyncServer::shared_conn_handler_t handler, const boost::system::error_code& ec)
{
	if (ec) return;

	handler->Start();

	auto newHandler = std::make_shared<ConnHandlerT>(*ioService_);
	acceptor_->async_accept(newHandler->Socket(), [=](auto ec)
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

