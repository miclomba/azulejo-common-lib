#include "config.h"

#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include <boost/process.hpp>

#include <cpprest/details/basic_types.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include "Interprocess/RESTServer.h"

using RESTServer = interprocess::RESTServer;

namespace
{
const std::wstring BASE_URI = U("http://localhost/");

web::json::value CreateJSONObj()
{
	web::json::value value = web::json::value::object();
	value[L"one"] = web::json::value::string(L"100");
	value[L"two"] = web::json::value::string(L"200");
	return value;
}

void VerifyResponse(const web::json::value& response, const utility::string_t& expectedResponse)
{
	utility::string_t responseStr = response.serialize();
	EXPECT_TRUE(responseStr.compare(expectedResponse) == 0);
}

struct RESTServerT : public RESTServer
{
	RESTServerT(const std::wstring& baseURI) : RESTServer(baseURI) {}

	void AcceptHandler() override { calledAcceptHandler_ = true; }
	void POSTHandlerPublic(const web::json::value& jValue, web::json::value& answer) { POSTHandler(jValue, answer); }
	void PUTHandlerPublic(const web::json::value& jValue, web::json::value& answer) { PUTHandler(jValue, answer); }
	void DELHandlerPublic(const web::json::value& jValue, web::json::value& answer) { DELHandler(jValue, answer); }

	bool CalledAcceptHandler() const { return calledAcceptHandler_; }

private:
	bool calledAcceptHandler_{ false };
};
} // end namespace

TEST(RESTServer, Construct)
{
	EXPECT_NO_THROW(RESTServer server(BASE_URI));
}

TEST(RESTServer, ConstructThrows)
{
	EXPECT_THROW(RESTServer server(U("")), std::invalid_argument);
}

TEST(RESTServer, GetListener)
{
	RESTServer server(BASE_URI);

	EXPECT_NO_THROW(server.GetListener());
	const web::http::experimental::listener::http_listener& httpListener = server.GetListener();
	utility::string_t baseURI = httpListener.uri().to_string();
	EXPECT_EQ(baseURI.compare(BASE_URI), 0);
}

TEST(RESTServer, Listen)
{
	RESTServer server(BASE_URI);

	EXPECT_NO_THROW(server.Listen());
}

TEST(RESTServer, AcceptHandler)
{
	RESTServerT server(BASE_URI);

	EXPECT_NO_THROW(server.Listen());
	EXPECT_TRUE(server.CalledAcceptHandler());
}

TEST(RESTServer, POSTHandler)
{
	RESTServerT server(BASE_URI);

	web::json::value answer;

	web::json::value request = web::json::value::array();
	request[0] = web::json::value::string(L"one");
	request[1] = web::json::value::string(L"two");
	request[2] = web::json::value::string(L"three");

	server.POSTHandlerPublic(request, answer);
	VerifyResponse(answer, L"{\"one\":\"<nil>\",\"three\":\"<nil>\",\"two\":\"<nil>\"}");
}

TEST(RESTServer, PUTHandler)
{
	RESTServerT server(BASE_URI);

	web::json::value answer;

	web::json::value request = web::json::value::object();
	request[L"one"] = web::json::value::string(L"100");
	request[L"two"] = web::json::value::string(L"200");

	server.PUTHandlerPublic(request, answer);
	VerifyResponse(answer, L"{\"one\":\"<put>\",\"two\":\"<put>\"}");
}

TEST(RESTServer, DELHandler)
{
	RESTServerT server(BASE_URI);

	web::json::value answer;

	web::json::value request = web::json::value::array();
	request[0] = web::json::value::string(L"one");

	server.DELHandlerPublic(request, answer);
	VerifyResponse(answer, L"{\"one\":\"<failed>\"}");
}

