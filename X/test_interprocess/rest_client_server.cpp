#include <iostream>
#include <string>

#include <cpprest/json.h>

#include "config.h"

#include "Interprocess/RESTClient.h"
#include "Interprocess/RESTServer.h"

void display_json_client(
	web::json::value const& jvalue,
	utility::string_t const& prefix)
{
	std::wcout << prefix << jvalue.serialize() << std::endl;
}

#ifdef USER_REST_SERVER_MAIN
int main(int argc, char* argv[])
#else
int fake_main(int argc, char* argv[])
#endif
{
	if (argc < 2)
	{
		std::cerr << "Usage: test_interprocess rest_server\n";
		std::cerr << "Usage: test_interprocess rest_client\n";
		return 1;
	}

	if (std::string(argv[1]) == "rest_server")
	{
		interprocess::RESTServer server;

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
	else if (std::string(argv[1]) == "rest_client")
	{
		interprocess::RESTClient client;

		auto putvalue = web::json::value::object();
		putvalue[L"one"] = web::json::value::string(L"100");
		putvalue[L"two"] = web::json::value::string(L"200");

		std::wcout << L"\nPUT (add values)\n";
		display_json_client(putvalue, L"S: ");
		client.make_request(web::http::methods::PUT, putvalue);

		auto getvalue = web::json::value::array();
		getvalue[0] = web::json::value::string(L"one");
		getvalue[1] = web::json::value::string(L"two");
		getvalue[2] = web::json::value::string(L"three");

		std::wcout << L"\nPOST (get some values)\n";
		display_json_client(getvalue, L"S: ");
		client.make_request(web::http::methods::POST, getvalue);

		auto delvalue = web::json::value::array();
		delvalue[0] = web::json::value::string(L"one");

		std::wcout << L"\nDELETE (delete values)\n";
		display_json_client(delvalue, L"S: ");
		client.make_request(web::http::methods::DEL, delvalue);

		std::wcout << L"\nPOST (get some values)\n";
		display_json_client(getvalue, L"S: ");
		client.make_request(web::http::methods::POST, getvalue);

		auto nullvalue = web::json::value::null();

		std::wcout << L"\nGET (get all values)\n";
		display_json_client(nullvalue, L"S: ");
		client.make_request(web::http::methods::GET, nullvalue);
	}

	return 0;
}

