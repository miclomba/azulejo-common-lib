#include "config.h"

#include <limits>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "Server.h"

using networking::Server;

namespace
{
const int PORT = 13;
} // end namespace

TEST(IServerEntity, Construct)
{
	EXPECT_NO_THROW(Server server(PORT));
}

TEST(IServerEntity, ConstructThrowsWhenPortIsNegative)
{
	EXPECT_THROW(Server server(-1), std::runtime_error);
}

TEST(IServerEntity, ConstructThrowsWhenPortIsTooLarge)
{
	int port = std::numeric_limits<unsigned short>::max() + 1;
	EXPECT_THROW(Server server(port), std::runtime_error);
}
