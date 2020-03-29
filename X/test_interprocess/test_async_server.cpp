#include "config.h"

#include <fstream>
#include <stdexcept>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
// this header must always follow the include of <winsock2.h>
#include <iphlpapi.h>
#endif

/*
	This test project uses gmock google project. To install gmock:
	from nuget:
		Install-Package gmock -Version 1.10.0
	then add in packages.config:
		<package id="gmock" version="1.10.0" targetFramework="native" />
	and add in vcxproj add to ExtensionTargets:
		<Import Project="..\packages\gmock.1.10.0\build\native\gmock.targets" Condition="Exists('..\packages\gmock.1.10.0\build\native\gmock.targets')" />
*/
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/asio/io_context.hpp>

#include "ChatHandler.h"
#include "interprocess/AsyncServer.h"

using ::testing::Return;
using interprocess::AsyncServer;
using networking::ChatHandler;

namespace
{
WSADATA wsaData;

const int TWO_THREADS = 2;
const int ZERO_THREADS = 0;
const uint16_t PORT = 1500;
} // end namespace

TEST(AsyncServer, Construct)
{
	EXPECT_NO_THROW(AsyncServer<ChatHandler> server(TWO_THREADS));
}

TEST(AsyncServer, ConstructThrows)
{
	EXPECT_THROW(AsyncServer<ChatHandler> server(ZERO_THREADS), std::runtime_error);
}

TEST(AsyncServer, GetNumThreads)
{
	AsyncServer<ChatHandler> server(TWO_THREADS);
	const size_t numThreads = server.GetNumThreads();
	EXPECT_EQ(numThreads, TWO_THREADS);
}

TEST(AsyncServer, Start)
{
	AsyncServer<ChatHandler> server(TWO_THREADS);
	//EXPECT_NO_THROW(server.Start(PORT));
}
