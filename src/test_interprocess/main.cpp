#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Interprocess/RESTServer.h"

#include "test_interprocess/config.h"

namespace
{
	const std::string REST_SERVER_PARAM = "rest_server";
	const std::wstring REST_SERVER_URI = L"http://localhost/restdemo";
}

#if defined(USER_TCP_SERVER_MAIN) || defined(USER_REST_SERVER_MAIN)
int fake_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	if (argc == 2 && std::string(argv[1]).compare(REST_SERVER_PARAM) == 0)
	{
		interprocess::RESTServer server(REST_SERVER_URI);

		try
		{
			server.Listen();
			while (true)
				;
		}
		catch (const std::exception &e)
		{
			std::wcout << e.what() << std::endl;
		}
	}
	else
	{
		::testing::InitGoogleMock(&argc, argv);
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}

	return 0;
}