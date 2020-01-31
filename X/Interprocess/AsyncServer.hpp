
#define TEMPLATE_T template<typename ConnHandlerT>
#define AsyncServer_t AsyncServer<ConnHandlerT>

TEMPLATE_T
AsyncServer_t::AsyncServer(const size_t numThreads) :
	numThreads_(numThreads),
	acceptor_(ioService_)
{
	if (numThreads_ == 0)
		throw std::runtime_error("Cannot construct AsyncServer because number of threads is invalid");
}

TEMPLATE_T
AsyncServer_t::~AsyncServer() = default;

TEMPLATE_T
void AsyncServer_t::Start(const uint16_t port)
{
	using boost::asio::ip::tcp;

	tcp::endpoint endpoint(tcp::v4(), port);
	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(tcp::acceptor::reuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.listen();

	auto handler = std::make_shared<ConnHanlderT>(ioService_);
	acceptor_.async_accept(handler->Socket(), [=](auto ec) 
	{
		HandleNewConnection(handler, ec);
	});

	for (size_t i = 0; i < numThreads_; ++i)
		threadPool_.emplace_back([=]() { ioService_.run(); });
}

TEMPLATE_T
void AsyncServer_t::HandleNewConnection(AsyncServer::shared_conn_handler_t handler, const boost::system::error_code& ec)
{
	if (ec) return;

	handler->Start();

	auto newHandler = std::make_shared<ConnHandlerT>(ioService_);
	acceptor_.async_accept(newHandler->Socket(), [=](auto ec)
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

