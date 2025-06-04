#include <exception>
#include <iostream>
#include <string>

#include <cpprest/details/basic_types.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>

#include "test_interprocess/config.h"

#include "Interprocess/RESTClient.h"
#include "Interprocess/RESTServer.h"

namespace
{
	const std::string CLIENT_PARAM = "rest_client";
	const std::string SERVER_PARAM = "rest_server";

	void PrintJSONRequest(const utility::string_t &prefix, const web::json::value &jValue = web::json::value::null())
	{
		std::wcout << '\n'
				   << prefix << '\n';
		std::wcout << L"S: " << jValue.serialize() << '\n';
	}

	void PrintJSONResponse(const web::json::value &jValue)
	{
		std::wcout << L"R: " << jValue.serialize() << std::endl;
	}
} // end namespace

#ifdef USER_REST_SERVER_MAIN
int main(int argc, char *argv[])
#else
int fake_rest_main(int argc, char *argv[])
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
			while (true)
				;
		}
		catch (const std::exception &e)
		{
			std::wcout << e.what() << std::endl;
		}
	}
	else if (std::string(argv[1]).compare(CLIENT_PARAM) == 0)
	{
		web::json::value response;
		interprocess::RESTClient client(U("http://localhost"));

		web::json::value putvalue = web::json::value::object();
		putvalue[L"one"] = web::json::value::string(L"100");
		putvalue[L"two"] = web::json::value::string(L"200");
		PrintJSONRequest(L"PUT (add values)", putvalue);
		response = client.PUTRequest(L"/restdemo", putvalue);
		PrintJSONResponse(response);

		web::json::value getvalue = web::json::value::array();
		getvalue[0] = web::json::value::string(L"one");
		getvalue[1] = web::json::value::string(L"two");
		getvalue[2] = web::json::value::string(L"three");
		PrintJSONRequest(L"POST (get some values)", getvalue);
		response = client.POSTRequest(L"/restdemo", getvalue);
		PrintJSONResponse(response);

		web::json::value delvalue = web::json::value::array();
		delvalue[0] = web::json::value::string(L"one");
		PrintJSONRequest(L"DELETE (delete values)", delvalue);
		response = client.DELRequest(L"/restdemo", delvalue);
		PrintJSONResponse(response);

		PrintJSONRequest(L"POST (get some values)", getvalue);
		response = client.POSTRequest(L"/restdemo", getvalue);
		PrintJSONResponse(response);

		PrintJSONRequest(L"GET (get all values)");
		response = client.GETRequest(L"/restdemo");
		PrintJSONResponse(response);
	}

	return 0;
}
