#include "config.h"

#include <limits>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "Server.h"

using networking::Server;
using boost::asio::io_service;

namespace
{
const int PORT = 13;
} // end namespace

TEST(IServer, Construct)
{
	EXPECT_NO_THROW(Server server(PORT));
}

TEST(IServer, ConstructThrowsWhenPortIsNegative)
{
	EXPECT_THROW(Server server(-1), std::runtime_error);
}

TEST(IServer, ConstructThrowsWhenPortIsTooLarge)
{
	int port = std::numeric_limits<unsigned short>::max() + 1;
	EXPECT_THROW(Server server(port), std::runtime_error);
}

TEST(IServer, GetIOService)
{
	Server server(PORT);
	const io_service* ioService = server.GetIOServiceProtected();
	EXPECT_TRUE(ioService);
}
