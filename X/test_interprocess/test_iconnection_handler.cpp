#include "config.h"

#include <gtest/gtest.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Interprocess/IConnectionHandler.h"

using boost::asio::io_context;
using boost::asio::ip::tcp;
using interprocess::IConnectionHandler;

namespace
{
struct MockHandler : public IConnectionHandler {
	MockHandler(io_context& context) : IConnectionHandler(context) {}
	void Start() override { ++startCount_; }

	size_t GetStartCount() { return startCount_; }

private:
	size_t startCount_{ 0 };
};
} // end namespace

TEST(IConnectionHandler, Construct)
{
	io_context context;
	EXPECT_NO_THROW(MockHandler handler(context));
}

TEST(IConnectionHandler, Start)
{
	io_context context;
	MockHandler handler(context);

	EXPECT_EQ(handler.GetStartCount(), 0);
	handler.Start();
	EXPECT_EQ(handler.GetStartCount(), 1);
}

TEST(IConnectionHandler, Socket)
{
	io_context context;
	MockHandler handler(context);

	tcp::socket& soket = handler.Socket();
	EXPECT_EQ(&(soket.get_io_context()), &context);
}

TEST(IConnectionHandler, IOService)
{
	io_context context;
	MockHandler handler(context);

	io_context& contextRef = handler.IOService();
	EXPECT_EQ(&(contextRef), &context);
}
