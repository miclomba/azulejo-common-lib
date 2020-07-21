#include "HTTP.h"

#include <codecvt>
#include <exception>
#include <functional>
#include <locale>
#include <memory>
#include <string>
#include "Config/filesystem.hpp"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

using interprocess::HTTP;

namespace
{
std::wstring WStr(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(s);
}
} // end namespace

std::wstring HTTP::GetJSON(const Path& uri)
{
    std::wstring result;

	auto requestJson = http_client(WStr(uri.string()))	
    .request(methods::GET, uri_builder(WStr("api")).append_path(WStr("users")).append_query(WStr("id"), 1).to_string())
	.then([](http_response response) {
		// Check the status code.
		if (response.status_code() != 200) {
			throw std::runtime_error("Returned " + std::to_string(response.status_code()));
		}
 
		// Convert the response body to JSON object.
		return response.extract_json();
	})
	.then([](json::value jsonObject) { // Get the data field.
		return jsonObject[WStr("data")];
	})
	.then([&result](json::value jsonObject) { // Parse the user details.
        result = jsonObject[WStr("first_name")].as_string()
		+ U(" ") + jsonObject[WStr("last_name")].as_string()
		+ U(" (") + std::to_wstring(jsonObject[WStr("id")].as_integer()) + U(")");
	});
 
	// Wait for the concurrent tasks to finish.
	try {
		requestJson.wait();
	} catch (const std::exception &e) {
		throw std::runtime_error(std::string("Error exception: ") + e.what());
	}

    return result;
}

void HTTP::Get(const Path& uri, const Path& resultPath)
{
    auto fileStream = std::make_shared<concurrency::streams::ostream>();

    // Open stream to output file.
    pplx::task<void> requestTask = 
        concurrency::streams::fstream::open_ostream(WStr(resultPath.string()))
    .then([=](concurrency::streams::ostream outFile)
    {
        *fileStream = outFile;

        // Create http_client to send the request.
        web::http::client::http_client client(WStr(uri.string()));

        // Build request URI api/users
        auto builder = web::uri_builder(WStr("api")).append_path(WStr("users"));

        // start the request
        return client.request(web::http::methods::GET, builder.to_string());
    })
    .then([=](web::http::http_response response)
    {
        if (response.status_code() != 200) 
            throw std::runtime_error("Returned " + std::to_string(response.status_code()));
        return response.body().read_to_end(fileStream->streambuf());
    })
    .then([=](size_t)
    {
        return fileStream->close();
    });

    try
    {
        requestTask.wait();
    }
    catch (const std::exception& e)
    {
        std::cout << "Error exception: " << e.what() << std::endl;
    }
}
