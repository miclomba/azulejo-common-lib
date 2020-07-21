#include "Config/filesystem.hpp" 

#include <string>

#include "config.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Interprocess/HTTP.h"

using interprocess::HTTP;

namespace
{
const std::string URI = "https://reqres.in";
const std::string RESULTS_JSON = "users.json";
const fs::path RESULTS_PATH = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY / RESULTS_JSON;
const std::wstring RESULT_RECORD = U("George Bluth (1)");

struct HTTPF : public testing::Test
{
	HTTPF()
	{
		Clear();
	}

	~HTTPF()
	{
		Clear();
	}

private:
	void Clear()
	{
		if (fs::exists(RESULTS_PATH))
			fs::remove(RESULTS_PATH);
	}
};
}

TEST_F(HTTPF, Get)
{
	EXPECT_FALSE(fs::exists(RESULTS_PATH));

	HTTP request;
	request.Get(URI, RESULTS_PATH);

	EXPECT_TRUE(fs::exists(RESULTS_PATH));
}

TEST(HTTP, GetJSON)
{
	HTTP request;
	std::wstring result = request.GetJSON(URI);

	EXPECT_EQ(result.compare(RESULT_RECORD), 0);
}