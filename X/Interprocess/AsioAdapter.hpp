#define TEMPLATE_T template<typename PODType>
#define AsioAdapter_t AsioAdapter<PODType>

TEMPLATE_T
AsioAdapter_t::AsioAdapter() = default;

TEMPLATE_T
AsioAdapter_t::~AsioAdapter() = default;

TEMPLATE_T
void AsioAdapter_t::AsyncReadUntil(
	boost::asio::ip::tcp::socket& socket,
	boost::asio::streambuf& messageBuffer,
	const char UNTIL_CONDITION,
	std::function<void(const boost::system::error_code& error, size_t bytesTransferred)> handler
)
{
	boost::asio::async_read_until(socket, messageBuffer, UNTIL_CONDITION, handler);
}

TEMPLATE_T
void AsioAdapter_t::AsyncWrite(
	boost::asio::ip::tcp::socket& socket,
	boost::asio::mutable_buffer& messageBuffer,
	std::function<void(const boost::system::error_code& error, size_t)> handler
)
{
	boost::asio::async_write(socket, messageBuffer, handler);
}

#undef TEMPLATE_T 
#undef AsioAdapter_t 