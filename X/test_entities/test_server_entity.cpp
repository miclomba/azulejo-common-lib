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

TEST(IServerEntity, CopyConstruct)
{
	Server server(PORT);
	EXPECT_NO_THROW(Server copy(server));
}

TEST(IServerEntity, CopyAssign)
{
	Server server(PORT);
	Server copy(PORT);
	EXPECT_NO_THROW(copy = server);
}

TEST(IServerEntity, MoveConstruct)
{
	Server server(PORT);
	EXPECT_NO_THROW(Server copy(std::move(server)));
}

TEST(IServerEntity, MoveAssign)
{
	Server server(PORT);
	Server copy(PORT);
	EXPECT_NO_THROW(copy = std::move(server));
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
