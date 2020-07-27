#include "config.h"

#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include <cpprest/details/basic_types.h>

#include "Interprocess/RESTClient.h"

namespace
{
const std::wstring BASE_URI = U("http://localhost/");
const std::wstring API_URI = U("api");

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

	web::json::value get() 
	{ 
		calledGet_++;
		return web::json::value(); 
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

private:
	static int calledThen_;
	static int calledGet_;
};

int MockResponseTask::calledGet_ = 0;
int MockResponseTask::calledThen_ = 0;

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
	MockRequestTaskBad(concurrency::task<web::http::http_response> task) :
		MockRequestTask(task)
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
using MockClientBad = interprocess::RESTClient<MockRequestTaskBad, MockResponseTask, MockResultTask>;
using RESTClient = interprocess::RESTClient<
	concurrency::task<web::http::http_response>,
	concurrency::task<web::json::value>,
	concurrency::task<void>
>;

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
	const web::http::client::http_client& httpClient = client.GetClient();
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

	EXPECT_EQ(MockRequestTask::GetCounts(), 2);
	EXPECT_EQ(MockResponseTask::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}

TEST(RESTClient, GETRequestBadResponse)
{
	MockRequestTask::ResetCounts();
	MockResponseTask::ResetCounts();
	MockResultTask::ResetCounts();

	MockClientBad client(BASE_URI);

	web::json::value responseJSON = client.GETRequest(API_URI);

	EXPECT_EQ(MockRequestTask::GetCounts(), 2);
	EXPECT_EQ(MockResponseTask::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}

TEST(RESTClient, HEADRequest)
{
	MockRequestTask::ResetCounts();
	MockResponseTask::ResetCounts();
	MockResultTask::ResetCounts();

	MockClient client(BASE_URI);

	web::json::value responseJSON = client.HEADRequest(API_URI);

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

	web::json::value requestJSON;
	web::json::value responseJSON = client.POSTRequest(API_URI, requestJSON);

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

	web::json::value requestJSON;
	web::json::value responseJSON = client.PUTRequest(API_URI, requestJSON);

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

	web::json::value requestJSON;
	web::json::value responseJSON = client.DELRequest(API_URI, requestJSON);

	EXPECT_EQ(MockRequestTask::GetCounts(), 2);
	EXPECT_EQ(MockResponseTask::GetCounts(), 2);
	EXPECT_EQ(MockResultTask::GetCounts(), 1);
}


