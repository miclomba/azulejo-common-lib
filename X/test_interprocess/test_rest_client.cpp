#include "config.h"

#include <memory>
#include <stdexcept>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Interprocess/RESTClient.h"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

using interprocess::RESTClient;

namespace
{
struct MockHandler : public RESTClient {
	MockHandler() : 
		RESTClient(L"") {}
/*
	void StartApplication(IConnectionHandler::shared_conn_handler_t) override { ++startCount_; }

	static void ResetStartCount() { startCount_ = 0; }
	static size_t GetStartCount() { return startCount_; }
*/
private:
	static size_t startCount_;
};

size_t MockHandler::startCount_ = 0;

struct MockAcceptor : public RESTClient 
{
/*
	MOCK_METHOD2(async_accept, void(tcp::socket&, std::function<void(const error_code&)>));

	MockAcceptor(io_context& context) : tcp::acceptor(context), context_(context) {}

	void AsyncAcceptImpl(tcp::socket& socket, std::function<void(const error_code&)> handler)
	{
		if (CalledAcceptMoreThanOnce()) return;
		handler(error_code());
	}

	void AsyncAcceptImplWithErrorParam(tcp::socket& socket, std::function<void(const error_code&)> handler)
	{
		if (CalledAcceptMoreThanOnce()) return;
		handler(errc::make_error_code(errc::not_supported));
	}

private:
	bool CalledAcceptMoreThanOnce() { return acceptCount_++ > 0; }
	size_t acceptCount_{ 0 };
	io_context& context_;
	*/
};
} // end namespace

TEST(RESTClient, Start)
{
/*
	io_context context;
	tcp::endpoint endPoint(tcp::v4(), PORT);

	AsyncServer<MockHandler, MockAcceptor> server(context, TWO_THREADS);

	EXPECT_CALL(server.GetAcceptor(), async_accept(_, _)).
		Times(2).
		WillRepeatedly(Invoke(&server.GetAcceptor(), &MockAcceptor::AsyncAcceptImpl));

	EXPECT_NO_THROW(server.Start(endPoint));
*/
}

