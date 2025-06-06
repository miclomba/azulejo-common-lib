#include "test_interprocess/config.h"

#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include <boost/process.hpp>

#include <cpprest/details/basic_types.h>

#include "Interprocess/RESTClient.h"

namespace
{
	const std::wstring BASE_URI = U("http://localhost/");
	const std::wstring API_URI = U("api");
	const std::wstring EXPECTED_JSON_RESPONSE = L"{\"one\":\"100\",\"two\":\"200\"}";
	const std::wstring ERROR_JSON_RESPONSE = L"\"ERROR: RESTClient: \"";

	utility::string_t PrintJSONResponse(const web::json::value &jValue)
	{
		return jValue.serialize();
	}

	void VerifyResponse(const web::json::value &response, const utility::string_t &expectedResponse)
	{
		utility::string_t responseStr = response.serialize();
		EXPECT_TRUE(responseStr.compare(expectedResponse) == 0);
	}

	web::json::value CreateJSONObj()
	{
		web::json::value value = web::json::value::object();
		value[L"one"] = web::json::value::string(L"100");
		value[L"two"] = web::json::value::string(L"200");
		return value;
	}

	struct MockResultTask
	{
		MockResultTask() {}
		MockResultTask(concurrency::task<void>) {}

		void wait()
		{
			calledWait_++;
		}

		static int GetCounts()
		{
			return calledWait_;
		}

		static void ResetCounts()
		{
			calledWait_ = 0;
		}

	private:
		static int calledWait_;
	};

	int MockResultTask::calledWait_ = 0;

	struct MockResponseTask
	{
		MockResponseTask() {}
		MockResponseTask(concurrency::task<web::json::value>) {}

		virtual web::json::value get()
		{
			calledGet_++;
			return CreateJSONObj();
		}

		MockResultTask then(std::function<void(MockResponseTask jsonResponse)> handler)
		{
			calledThen_++;
			handler(MockResponseTask());
			return MockResultTask();
		}

		static int GetCounts()
		{
			return calledThen_ + calledGet_;
		}

		static void ResetCounts()
		{
			calledThen_ = 0;
			calledGet_ = 0;
		}

	protected:
		static int calledThen_;
		static int calledGet_;
	};

	int MockResponseTask::calledGet_ = 0;
	int MockResponseTask::calledThen_ = 0;

	struct MockResponseTaskBad : public MockResponseTask
	{
		MockResponseTaskBad() {}
		MockResponseTaskBad(concurrency::task<web::json::value>) {}
		MockResponseTaskBad(MockResponseTask &&) {}

		web::json::value get() override
		{
			calledGet_++;
			throw web::http::http_exception("");
		}
	};

	struct MockRequestTask
	{
		MockRequestTask() {}
		MockRequestTask(concurrency::task<web::http::http_response>)
		{
			constructedFromTask_++;
		}

		MockResponseTask then(std::function<MockResponseTask(web::http::http_response httpResponse)> handler)
		{
			calledThen_++;

			web::http::http_response response;
			response.set_status_code(web::http::status_codes::OK);

			return handler(response);
		}

		static int GetCounts()
		{
			return calledThen_ + constructedFromTask_;
		}

		static void ResetCounts()
		{
			calledThen_ = 0;
			constructedFromTask_ = 0;
		}

	protected:
		static int calledThen_;
		static int constructedFromTask_;
	};

	int MockRequestTask::calledThen_ = 0;
	int MockRequestTask::constructedFromTask_ = 0;

	struct MockRequestTaskBad : public MockRequestTask
	{
		MockRequestTaskBad() : MockRequestTask() {}
		MockRequestTaskBad(concurrency::task<web::http::http_response> task) : MockRequestTask(task)
		{
		}

		MockResponseTask then(std::function<MockResponseTask(web::http::http_response httpResponse)> handler)
		{
			calledThen_++;

			web::http::http_response response;
			response.set_status_code(web::http::status_codes::BadRequest);

			return handler(response);
		}
	};
} // end namespace

using MockClient = interprocess::RESTClient<MockRequestTask, MockResponseTask, MockResultTask>;
using MockClientBadRequest = interprocess::RESTClient<MockRequestTaskBad, MockResponseTask, MockResultTask>;
using MockClientBadResponse = interprocess::RESTClient<MockRequestTask, MockResponseTaskBad, MockResultTask>;
using RESTClient = interprocess::RESTClient<
	concurrency::task<web::http::http_response>,
	concurrency::task<web::json::value>,
	concurrency::task<void>>;

TEST(RESTClient, Construct)
{
	EXPECT_NO_THROW(RESTClient client(BASE_URI));
}

TEST(RESTClient, ConstructThrows)
{
	EXPECT_THROW(RESTClient client(U("")), std::invalid_argument);
}

TEST(RESTClient, GetClient)
{
	RESTClient client(BASE_URI);

	EXPECT_NO_THROW(client.GetClient());
	const web::http::client::http_client &httpClient = client.GetClient();
	utility::string_t baseURI = httpClient.base_uri().to_string();
	EXPECT_EQ(baseURI.compare(BASE_URI), 0);
}

TEST(RESTClient, GETRequest)
{
	MockRequestTask::ResetCounts();
	MockResponseTask::ResetCounts();
	MockResultTask::ResetCounts();

	MockClient client(BASE_URI);

	web::json::value responseJSON = client.GETRequest(API_URI);
	VerifyResponse(responseJSON, EXPECTED_JSON_RESPONSE);

	EXPECT_EQ(MockRequestTask::GetCounts(), 2);
	EXPECT_EQ(MockResponseTask::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}

TEST(RESTClient, GETRequestBadRequest)
{
	MockRequestTaskBad::ResetCounts();
	MockResponseTask::ResetCounts();
	MockResultTask::ResetCounts();

	MockClientBadRequest client(BASE_URI);

	web::json::value responseJSON = client.GETRequest(API_URI);
	VerifyResponse(responseJSON, EXPECTED_JSON_RESPONSE);

	EXPECT_EQ(MockRequestTaskBad::GetCounts(), 2);
	EXPECT_EQ(MockResponseTask::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}

TEST(RESTClient, GETRequestBadResponse)
{
	MockRequestTask::ResetCounts();
	MockResponseTaskBad::ResetCounts();
	MockResultTask::ResetCounts();

	MockClientBadResponse client(BASE_URI);

	web::json::value responseJSON = client.GETRequest(API_URI);
	VerifyResponse(responseJSON, ERROR_JSON_RESPONSE);

	EXPECT_EQ(MockRequestTask::GetCounts(), 2);
	EXPECT_EQ(MockResponseTaskBad::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}

TEST(RESTClient, HEADRequest)
{
	MockRequestTask::ResetCounts();
	MockResponseTask::ResetCounts();
	MockResultTask::ResetCounts();

	MockClient client(BASE_URI);

	web::json::value responseJSON = client.HEADRequest(API_URI);
	VerifyResponse(responseJSON, EXPECTED_JSON_RESPONSE);

	EXPECT_EQ(MockRequestTask::GetCounts(), 2);
	EXPECT_EQ(MockResponseTask::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}

TEST(RESTClient, POSTRequest)
{
	MockRequestTask::ResetCounts();
	MockResponseTask::ResetCounts();
	MockResultTask::ResetCounts();

	MockClient client(BASE_URI);

	web::json::value responseJSON = client.POSTRequest(API_URI, CreateJSONObj());
	VerifyResponse(responseJSON, EXPECTED_JSON_RESPONSE);

	EXPECT_EQ(MockRequestTask::GetCounts(), 2);
	EXPECT_EQ(MockResponseTask::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}

TEST(RESTClient, PUTRequest)
{
	MockRequestTask::ResetCounts();
	MockResponseTask::ResetCounts();
	MockResultTask::ResetCounts();

	MockClient client(BASE_URI);

	web::json::value responseJSON = client.PUTRequest(API_URI, CreateJSONObj());
	VerifyResponse(responseJSON, EXPECTED_JSON_RESPONSE);

	EXPECT_EQ(MockRequestTask::GetCounts(), 2);
	EXPECT_EQ(MockResponseTask::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}

TEST(RESTClient, DELRequest)
{
	MockRequestTask::ResetCounts();
	MockResponseTask::ResetCounts();
	MockResultTask::ResetCounts();

	MockClient client(BASE_URI);

	web::json::value responseJSON = client.DELRequest(API_URI, CreateJSONObj());
	VerifyResponse(responseJSON, EXPECTED_JSON_RESPONSE);

	EXPECT_EQ(MockRequestTask::GetCounts(), 2);
	EXPECT_EQ(MockResponseTask::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}

#ifdef INTEGRATION_TEST
TEST(RESTClient, Integration)
{
	// boost::process::child proc("C:/Users/jdoe/azulejo-common-lib/start_win_devenv.bat rest_server");
	// while (!proc.running()) {}
	// ASSERT_TRUE(proc.valid());
	// proc.detach();

	web::json::value response, putvalue, getvalue, delvalue;
	interprocess::RESTClient client(BASE_URI);

	putvalue = web::json::value::object();
	putvalue[L"one"] = web::json::value::string(L"100");
	putvalue[L"two"] = web::json::value::string(L"200");
	response = client.PUTRequest(L"/restdemo", putvalue);
	VerifyResponse(response, L"{\"one\":\"<put>\",\"two\":\"<put>\"}");

	getvalue = web::json::value::array();
	getvalue[0] = web::json::value::string(L"one");
	getvalue[1] = web::json::value::string(L"two");
	getvalue[2] = web::json::value::string(L"three");
	response = client.POSTRequest(L"/restdemo", getvalue);
	VerifyResponse(response, L"{\"one\":\"100\",\"three\":\"<nil>\",\"two\":\"200\"}");

	delvalue = web::json::value::array();
	delvalue[0] = web::json::value::string(L"one");
	response = client.DELRequest(L"/restdemo", delvalue);
	VerifyResponse(response, L"{\"one\":\"<deleted>\"}");

	response = client.POSTRequest(L"/restdemo", getvalue);
	VerifyResponse(response, L"{\"one\":\"<nil>\",\"three\":\"<nil>\",\"two\":\"200\"}");

	response = client.GETRequest(L"/restdemo");
	VerifyResponse(response, L"{\"two\":\"200\"}");

	// proc.terminate();
	// proc.wait();
}
#endif
