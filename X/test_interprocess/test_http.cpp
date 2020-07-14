#include "Config/filesystem.hpp" 

#include <string>

#include "config.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Interprocess/HTTP.h"

using interprocess::HTTP;

namespace
{
const std::string RESULTS_HTML = "results.html";
const fs::path RESULTS_PATH = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY / RESULTS_HTML;

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
	request.Get(RESULTS_PATH);

	EXPECT_TRUE(fs::exists(RESULTS_PATH));
}