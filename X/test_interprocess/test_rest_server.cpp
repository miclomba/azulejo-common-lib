#include "config.h"

#include <functional>
#include <map>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include <boost/process.hpp>

#include <cpprest/details/basic_types.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include "Interprocess/RESTServer.h"

using RESTServer = interprocess::RESTServer<web::http::experimental::listener::http_listener>;

namespace
{
const std::wstring BASE_URI = U("http://localhost/");
std::vector<web::http::method> METHODS = {
	web::http::methods::GET, web::http::methods::HEAD, web::http::methods::POST,
	web::http::methods::PUT, web::http::methods::DEL
};

web::json::value CreateJSONObj()
{
	web::json::value value = web::json::value::object();
	value[L"one"] = web::json::value::string(L"100");
	value[L"two"] = web::json::value::string(L"200");
	return value;
}

void AddJSONToDictionary(const web::json::value& jValue, std::map<utility::string_t, utility::string_t>& dictionary)
{
	for (const std::pair<utility::string_t, web::json::value>& e : jValue.as_object())
	{
		if (e.second.is_string())
		{
			utility::string_t key = e.first;
			utility::string_t value = e.second.as_string();
			dictionary[key] = value;
		}
	}
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
	void GETHandlerPublic(web::http::http_request request) { GETHandler(std::move(request)); }
	void HEADHandlerPublic(web::http::http_request request) { HEADHandler(std::move(request)); }

	bool CalledAcceptHandler() const { return calledAcceptHandler_; }
	std::map<utility::string_t, utility::string_t>& GetDictionaryPublic() { return GetDictionary(); }

private:
	bool calledAcceptHandler_{ false };
};

struct MockListener
{
	MockListener(const std::wstring& baseURI) {}

	MockListener& open() 
	{
		openCount_++;
		return *this; 
	}

	MockListener& then(std::function<void()>) 
	{
		thenCount_++;
		return *this; 
	}

	void wait() 
	{
		waitCount_++;
	}

	void support(web::http::method mtd, std::function<void(web::http::http_request request)>) 
	{
		methods_.push_back(mtd);
	}

	int OpenCount() const { return openCount_; }
	int ThenCount() const { return thenCount_; }
	int WaitCount() const { return waitCount_; }

	const std::vector<web::http::method>& Methods() const { return methods_; }

private:
	int openCount_{ 0 };
	int thenCount_{ 0 };
	int waitCount_{ 0 };
	
	std::vector<web::http::method> methods_;
};
} // end namespace

using MockRESTServer = interprocess::RESTServer<MockListener>;

TEST(RESTServer, Construct)
{
	MockRESTServer server(BASE_URI);
	const MockListener& listener = server.GetListener();

	const std::vector<web::http::method>& methods = listener.Methods();
	EXPECT_EQ(methods, METHODS);
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
	MockRESTServer server(BASE_URI);

	EXPECT_NO_THROW(server.Listen());
	const MockListener& listener = server.GetListener();

	EXPECT_EQ(listener.OpenCount(), 1);
	EXPECT_EQ(listener.ThenCount(), 1);
	EXPECT_EQ(listener.WaitCount(), 1);
}

TEST(RESTServer, AcceptHandler)
{
	RESTServerT server(BASE_URI);

	EXPECT_NO_THROW(server.Listen());
	EXPECT_TRUE(server.CalledAcceptHandler());
}

TEST(RESTServer, GETHandler)
{
	RESTServerT server(BASE_URI);

	web::http::http_request request;
	server.GETHandlerPublic(request);

	utility::string_t response(request.get_response().get().extract_string().get().c_str());
	EXPECT_TRUE(response.compare(L"{}") == 0);
}

TEST(RESTServer, GETHandlerWithData)
{
	RESTServerT server(BASE_URI);

	web::json::value data = web::json::value::object();
	data[L"one"] = web::json::value::string(L"100");
	data[L"two"] = web::json::value::string(L"200");

	AddJSONToDictionary(data, server.GetDictionaryPublic());

	web::http::http_request request;
	server.GETHandlerPublic(request);

	utility::string_t response(request.get_response().get().extract_string().get().c_str());
	EXPECT_TRUE(response.compare(L"{\"one\":\"100\",\"two\":\"200\"}") == 0);
}

TEST(RESTServer, HEADHandler)
{
	RESTServerT server(BASE_URI);

	web::http::http_request request;
	server.HEADHandlerPublic(request);

	utility::string_t response(request.get_response().get().extract_string().get().c_str());
	EXPECT_TRUE(response.compare(L"{}") == 0);
}

TEST(RESTServer, HEADHandlerWithData)
{
	RESTServerT server(BASE_URI);

	web::json::value data = web::json::value::object();
	data[L"one"] = web::json::value::string(L"100");
	data[L"two"] = web::json::value::string(L"200");

	AddJSONToDictionary(data, server.GetDictionaryPublic());

	web::http::http_request request;
	server.HEADHandlerPublic(request);

	utility::string_t response(request.get_response().get().extract_string().get().c_str());
	EXPECT_TRUE(response.compare(L"{}") == 0);
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

TEST(RESTServer, POSTHandlerWithData)
{
	RESTServerT server(BASE_URI);

	web::json::value answer;

	web::json::value request = web::json::value::array();
	request[0] = web::json::value::string(L"one");
	request[1] = web::json::value::string(L"two");
	request[2] = web::json::value::string(L"three");

	web::json::value data = web::json::value::object();
	data[L"one"] = web::json::value::string(L"100");
	data[L"two"] = web::json::value::string(L"200");

	AddJSONToDictionary(data, server.GetDictionaryPublic());

	server.POSTHandlerPublic(request, answer);
	VerifyResponse(answer, L"{\"one\":\"100\",\"three\":\"<nil>\",\"two\":\"200\"}");
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

TEST(RESTServer, PUTHandlerWithData)
{
	RESTServerT server(BASE_URI);

	web::json::value answer;

	web::json::value request = web::json::value::object();
	request[L"one"] = web::json::value::string(L"100");
	request[L"two"] = web::json::value::string(L"200");

	web::json::value data = web::json::value::object();
	data[L"one"] = web::json::value::string(L"100");
	data[L"two"] = web::json::value::string(L"200");

	AddJSONToDictionary(data, server.GetDictionaryPublic());

	server.PUTHandlerPublic(request, answer);
	VerifyResponse(answer, L"{\"one\":\"<updated>\",\"two\":\"<updated>\"}");
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

TEST(RESTServer, DELHandlerWithData)
{
	RESTServerT server(BASE_URI);

	web::json::value answer;

	web::json::value request = web::json::value::array();
	request[0] = web::json::value::string(L"one");

	web::json::value data = web::json::value::object();
	data[L"one"] = web::json::value::string(L"100");
	data[L"two"] = web::json::value::string(L"200");

	AddJSONToDictionary(data, server.GetDictionaryPublic());

	server.DELHandlerPublic(request, answer);
	VerifyResponse(answer, L"{\"one\":\"<deleted>\"}");
}

TEST(RESTServer, GetDictionary)
{
	RESTServerT server(BASE_URI);
	EXPECT_NO_THROW(server.GetDictionaryPublic());
	EXPECT_TRUE(server.GetDictionaryPublic().empty());
}

