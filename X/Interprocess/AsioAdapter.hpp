#define TEMPLATE_T template<typename PODType>
#define AsioAdapter_t AsioAdapter<PODType>

TEMPLATE_T
AsioAdapter_t::AsioAdapter() = default;

TEMPLATE_T
AsioAdapter_t::~AsioAdapter() = default;

TEMPLATE_T
void AsioAdapter_t::AsyncRead(
	boost::asio::ip::tcp::socket& socket,
	std::vector<PODType>& messageBuffer,
	const size_t bytesToRead,
	std::function<void(boost::system::error_code error, size_t bytesTransferred)> handler
)
{
	if (bytesToRead > messageBuffer.size() * sizeof(PODType))
		throw std::runtime_error("AsyncRead cannot read more bytes than available in the buffer");
	boost::asio::async_read(socket, boost::asio::buffer(messageBuffer.data(), bytesToRead), handler);
}

TEMPLATE_T
void AsioAdapter_t::AsyncWrite(
	boost::asio::ip::tcp::socket& socket,
	boost::asio::mutable_buffer& messageBuffer,
	std::function<void(boost::system::error_code error, size_t)> handler
)
{
	boost::asio::async_write(socket, messageBuffer, handler);
}

TEMPLATE_T
void AsioAdapter_t::AsyncConnect(
	boost::asio::ip::tcp::socket& socket,
	boost::asio::ip::tcp::resolver::results_type endpoint_iterator,
	std::function<void(boost::system::error_code error, boost::asio::ip::tcp::endpoint)> handler
)
{
	boost::asio::async_connect(socket, endpoint_iterator, handler);
}

#undef TEMPLATE_T 
#undef AsioAdapter_t 