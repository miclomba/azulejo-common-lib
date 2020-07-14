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
std::wstring ToWideString(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(s);
}
}

void HTTP::Get(const Path& resultsFile)
{
    auto fileStream = std::make_shared<concurrency::streams::ostream>();

    // Open stream to output file.
    pplx::task<void> requestTask = 
        concurrency::streams::fstream::open_ostream(ToWideString(resultsFile.string()))
    .then([=](concurrency::streams::ostream outFile)
    {
        *fileStream = outFile;

        // Create http_client to send the request.
        web::http::client::http_client client(U("http://www.bing.com/"));

        // Build request URI and start the request.
        web::uri_builder builder(U("/search"));
        builder.append_query(U("q"), U("cpprestsdk github"));
        return client.request(web::http::methods::GET, builder.to_string());
    })
    .then([=](web::http::http_response response)
    {
            std::cout << "Received response status code: " << response.status_code() << std::endl;

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
