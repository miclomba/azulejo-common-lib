#define TEMPLATE_T template<typename PacketT>
#define AsyncIO_t AsyncIO<PacketT>

TEMPLATE_T
AsyncIO_t::AsyncIO() = default;

TEMPLATE_T
AsyncIO_t::~AsyncIO() = default;

TEMPLATE_T
void AsyncIO_t::AsyncReadUntil(
	boost::asio::ip::tcp::socket& socket,
	boost::asio::streambuf& inPacket,
	const char UNTIL_CONDITION,
	std::function<void(const boost::system::error_code& error, size_t bytesTransferred)> handler
)
{
	boost::asio::async_read_until(socket, inPacket, UNTIL_CONDITION, handler);
}

TEMPLATE_T
void AsyncIO_t::AsyncWrite(
	boost::asio::ip::tcp::socket& socket,
	std::vector<PacketT>& packets,
	std::function<void(const boost::system::error_code& error, size_t)> handler
)
{
	boost::asio::async_write(socket, boost::asio::buffer(packets), handler);
}

#undef TEMPLATE_T 
#undef AsyncIO_t 