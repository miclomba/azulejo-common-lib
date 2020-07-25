#include <iostream>
#include <string>

#include <cpprest/json.h>

#include "config.h"

#include "Interprocess/RESTClient.h"
#include "Interprocess/RESTServer.h"

namespace
{
const std::string CLIENT_PARAM = "rest_client";
const std::string SERVER_PARAM = "rest_server";

void PrintJSON(const web::json::value& jvalue, const utility::string_t& prefix)
{
	std::wcout << '\n' << prefix << '\n';
	std::wcout << L"S: " << jvalue.serialize() << '\n';
}
} // end namespace

#ifdef USER_REST_SERVER_MAIN
int main(int argc, char* argv[])
#else
int fake_main(int argc, char* argv[])
#endif
{
	if (argc < 2)
	{
		std::cerr << "Usage: test_interprocess " + SERVER_PARAM + "\n";
		std::cerr << "Usage: test_interprocess " + CLIENT_PARAM + "\n";
		return 1;
	}

	if (std::string(argv[1]).compare(SERVER_PARAM) == 0)
	{
		interprocess::RESTServer server(L"http://localhost/restdemo");

		try
		{
			server.Listen();
			while (true);
		}
		catch (std::exception const& e)
		{
			std::wcout << e.what() << std::endl;
		}
	}
	else if (std::string(argv[1]).compare(CLIENT_PARAM) == 0)
	{
		interprocess::RESTClient client(U("http://localhost"));

		auto putvalue = web::json::value::object();
		putvalue[L"one"] = web::json::value::string(L"100");
		putvalue[L"two"] = web::json::value::string(L"200");
		PrintJSON(putvalue, L"PUT (add values)");
		client.MakeRequest(web::http::methods::PUT, L"/restdemo", putvalue);

		auto getvalue = web::json::value::array();
		getvalue[0] = web::json::value::string(L"one");
		getvalue[1] = web::json::value::string(L"two");
		getvalue[2] = web::json::value::string(L"three");
		PrintJSON(getvalue, L"POST (get some values)");
		client.MakeRequest(web::http::methods::POST, L"/restdemo", getvalue);

		auto delvalue = web::json::value::array();
		delvalue[0] = web::json::value::string(L"one");
		PrintJSON(delvalue, L"DELETE (delete values)");
		client.MakeRequest(web::http::methods::DEL, L"/restdemo", delvalue);

		PrintJSON(getvalue, L"POST (get some values)");
		client.MakeRequest(web::http::methods::POST, L"/restdemo", getvalue);

		auto nullvalue = web::json::value::null();
		PrintJSON(nullvalue, L"GET (get all values)");
		client.MakeRequest(web::http::methods::GET, L"/restdemo", nullvalue);
	}

	return 0;
}

