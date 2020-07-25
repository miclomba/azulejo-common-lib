#include "RESTClient.h"

#include <codecvt>
#include <exception>
#include <functional>
#include <iostream>
#include <locale>
#include <memory>
#include <string>
#include "Config/filesystem.hpp"

#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

using interprocess::RESTClient;

namespace
{
std::wstring WStr(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(s);
}

void PrintJSON(const web::json::value& jvalue, const utility::string_t& prefix)
{
    std::wcout << prefix << jvalue.serialize() << std::endl;
}

pplx::task<http_response> MakeTaskRequest(
    web::http::client::http_client& client,
    web::http::method mtd,
    const std::wstring& uri,
    web::json::value const& jvalue)
{
    return (mtd == methods::GET || mtd == methods::HEAD) ?
        client.request(mtd, uri) :
        client.request(mtd, uri, jvalue);
}
} // end namespace

RESTClient::RESTClient(const std::wstring& uri) :
    client_(uri)
{
}

void RESTClient::MakeRequest(web::http::method mtd, const std::wstring& uri, const web::json::value& jvalue)
{
    MakeTaskRequest(client_, mtd, uri, jvalue)
        .then([](http_response response)
        {
            if (response.status_code() == status_codes::OK)
                return response.extract_json();
            return pplx::task_from_result(json::value());
        })
        .then([](pplx::task<json::value> previousTask)
        {
            try
            {
                PrintJSON(previousTask.get(), L"R: ");
            }
            catch (http_exception const& e)
            {
                std::wcout << e.what() << std::endl;
            }
        })
        .wait();
}

