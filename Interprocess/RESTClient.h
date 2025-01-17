/**
 * @file RESTClient.h
 * @brief Declaration of the RESTClient class template for interacting with RESTful web servers.
 */

#ifndef interprocess_rest_client_h
#define interprocess_rest_client_h

#include <codecvt>
#include <functional>
#include <locale>
#include <string>
#include "Config/filesystem.h"

#include <cpprest/details/basic_types.h>
#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>

#include "config.h"

namespace interprocess {

/**
 * @class RESTClient
 * @brief A class template for interacting with RESTful web servers using the cpprest SDK.
 *
 * @tparam RequestTask The type representing the request task (default: pplx::task<web::http::http_response>).
 * @tparam ResponseTask The type representing the response task (default: pplx::task<web::json::value>).
 * @tparam ResultTask The type representing the result task (default: pplx::task<void>).
 */
template<
    typename RequestTask = pplx::task<web::http::http_response>,
    typename ResponseTask = pplx::task<web::json::value>,
    typename ResultTask = pplx::task<void>
>
class RESTClient
{
public:
    /**
     * @brief Constructor for the RESTClient class.
     * @param baseURI The base URI for the REST client.
     */
    RESTClient(const std::wstring& baseURI);

    /**
     * @brief Perform an HTTP GET request.
     * @param requestURI The URI for the GET request.
     * @return The JSON response from the server.
     */
    web::json::value GETRequest(const std::wstring& requestURI);

    /**
     * @brief Perform an HTTP HEAD request.
     * @param requestURI The URI for the HEAD request.
     * @return The JSON response from the server.
     */
    web::json::value HEADRequest(const std::wstring& requestURI);

    /**
     * @brief Perform an HTTP PUT request.
     * @param requestURI The URI for the PUT request.
     * @param jValue The JSON payload to send.
     * @return The JSON response from the server.
     */
    web::json::value PUTRequest(const std::wstring& requestURI, const web::json::value& jValue);

    /**
     * @brief Perform an HTTP POST request.
     * @param requestURI The URI for the POST request.
     * @param jValue The JSON payload to send.
     * @return The JSON response from the server.
     */
    web::json::value POSTRequest(const std::wstring& requestURI, const web::json::value& jValue);

    /**
     * @brief Perform an HTTP DELETE request.
     * @param requestURI The URI for the DELETE request.
     * @param jValue The JSON payload to send.
     * @return The JSON response from the server.
     */
    web::json::value DELRequest(const std::wstring& requestURI, const web::json::value& jValue);

    /**
     * @brief Get the HTTP client used by the REST client.
     * @return A constant reference to the HTTP client.
     */
    const web::http::client::http_client& GetClient() const;

protected:
    /**
     * @brief Create a request task for the HTTP client.
     * @param client The HTTP client to use for the request.
     * @param httpMethod The HTTP method to use (e.g., GET, POST).
     * @param requestURI The URI for the request.
     * @param jValue The JSON payload to send (if applicable).
     * @return A task representing the HTTP request.
     */
    virtual RequestTask MakeRequestTask(
        web::http::client::http_client& client,
        web::http::method httpMethod,
        const std::wstring& requestURI,
        web::json::value const& jValue
    );

    /**
     * @brief Create a response task from an HTTP response.
     * @param httpResponse The HTTP response to process.
     * @return A task representing the JSON response.
     */
    virtual ResponseTask MakeResponseTask(web::http::http_response&& httpResponse);

    /**
     * @brief Extract the JSON payload from a response task.
     * @param jsonResponse The response task containing the JSON payload.
     * @return The extracted JSON value.
     */
    virtual web::json::value GetJSON(ResponseTask&& jsonResponse);

private:
    /**
     * @brief Perform an HTTP request.
     * @param httpMethod The HTTP method to use (e.g., GET, POST).
     * @param uri The URI for the request.
     * @param jValue The JSON payload to send (default: null JSON).
     * @return The JSON response from the server.
     */
    web::json::value MakeRequest(
        web::http::method httpMethod,
        const std::wstring& uri,
        const web::json::value& jValue = web::json::value::null()
    );

    /** @brief HTTP client for making requests. */
    web::http::client::http_client client_;
};

#include "RESTClient.hpp"

} // end namespace interprocess

#endif  // interprocess_rest_client_h
