#include "config.h"

#include <limits>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "Client.h"

using networking::Client;

namespace
{
const int PORT = 13;
const std::string HOST = "localhost";
} // end namespace

TEST(IClientEntity, Construct)
{
	EXPECT_NO_THROW(Client client());
}

TEST(IClientEntity, RunThrowsWhenHostIsEmpty)
{
	Client client;
	EXPECT_THROW(client.Run("", PORT), std::runtime_error);
}

TEST(IClientEntity, RunThrowsWhenPortIsNegative)
{
	Client client;
	EXPECT_THROW(client.Run(HOST,-1), std::runtime_error);
}

TEST(IClientEntity, RunThrowsWhenPortIsTooLarge)
{
	Client client;
	int port = std::numeric_limits<unsigned short>::max() + 1;
	EXPECT_THROW(client.Run(HOST,port), std::runtime_error);
}
