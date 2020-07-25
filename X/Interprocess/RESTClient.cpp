#include "RESTClient.h"

#include <codecvt>
#include <functional>
#include <locale>
#include <string>
//#include "Config/filesystem.hpp"

//#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>

//using namespace utility;                    // Common utilities like string conversions
//using namespace web;                        // Common features like URIs and json.
//using namespace web::http;                  // Common HTTP functionality
//using namespace web::http::client;          // HTTP client features
//using namespace concurrency::streams;       // Asynchronous streams

using interprocess::RESTClient;

namespace
{
std::wstring WStr(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(s);
}

pplx::task<web::http::http_response> MakeTaskRequest(
    web::http::client::http_client& client,
    web::http::method httpMethod,
    const std::wstring& requestURI,
    web::json::value const& jValue)
{
    return (httpMethod == web::http::methods::GET || httpMethod == web::http::methods::HEAD) ?
        client.request(httpMethod, requestURI) :
        client.request(httpMethod, requestURI, jValue);
}
} // end namespace

RESTClient::RESTClient(const std::wstring& serverURI) :
    client_(serverURI)
{
}

web::json::value RESTClient::GETRequest(const std::wstring& requestURI)
{
    return RESTClient::MakeRequest(web::http::methods::GET, requestURI);
}

web::json::value RESTClient::HEADRequest(const std::wstring& requestURI)
{
    return RESTClient::MakeRequest(web::http::methods::HEAD, requestURI);
}

web::json::value RESTClient::PUTRequest(const std::wstring& requestURI, const web::json::value& jValue)
{
    return RESTClient::MakeRequest(web::http::methods::PUT, requestURI, jValue);
}

web::json::value RESTClient::POSTRequest(const std::wstring& requestURI, const web::json::value& jValue)
{
    return RESTClient::MakeRequest(web::http::methods::POST, requestURI, jValue);
}

web::json::value RESTClient::DELRequest(const std::wstring& requestURI, const web::json::value& jValue)
{
    return RESTClient::MakeRequest(web::http::methods::DEL, requestURI, jValue);
}

web::json::value RESTClient::MakeRequest(web::http::method httpMethod, const std::wstring& requestURI, const web::json::value& jValue)
{
    web::json::value response;

    MakeTaskRequest(client_, httpMethod, requestURI, jValue)
        .then([](web::http::http_response httpResponse)
        {
            if (httpResponse.status_code() == web::http::status_codes::OK)
                return httpResponse.extract_json();
            return pplx::task_from_result(web::json::value());
        })
        .then([&response](pplx::task<web::json::value> jsonResponse)
        {
            try
            {
                response = std::move(jsonResponse.get());
            }
            catch (web::http::http_exception const& e)
            {
                response = web::json::value(WStr("ERROR: RESTClient: ") + WStr(e.what()));
            }
        })
        .wait();

    return response;
}

