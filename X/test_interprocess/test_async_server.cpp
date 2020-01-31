#include "config.h"

#include <stdexcept>

#include <gtest/gtest.h>

#include "DerivedConnectionHandler.h"
#include "interprocess/AsyncServer.h"

using interprocess::AsyncServer;
using networking::DerivedConnectionHandler;

namespace
{
const int TWO_THREADS = 2;
const int ZERO_THREADS = 0;
const uint16_t PORT = 1500;
} // end namespace

TEST(AsyncServer, Construct)
{
	EXPECT_NO_THROW(AsyncServer<DerivedConnectionHandler> server(TWO_THREADS));
}

TEST(AsyncServer, ConstructThrows)
{
	EXPECT_THROW(AsyncServer<DerivedConnectionHandler> server(ZERO_THREADS), std::runtime_error);
}

TEST(AsyncServer, GetNumThreads)
{
	AsyncServer<DerivedConnectionHandler> server(TWO_THREADS);
	const size_t numThreads = server.GetNumThreads();
	EXPECT_EQ(numThreads, TWO_THREADS);
}

TEST(AsyncServer, Start)
{
	AsyncServer<DerivedConnectionHandler> server(TWO_THREADS);
	//EXPECT_NO_THROW(server.Start(PORT));
}
