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
//#include <cpprest/filestream.h>

#include "config.h"

namespace interprocess {

template<
    typename RequestTask = pplx::task<web::http::http_response>,
    typename ResponseTask = pplx::task<web::json::value>,
    typename ResultTask = pplx::task<void>
>
class RESTClient
{
public:
    RESTClient(const std::wstring& baseURI);

    web::json::value GETRequest(const std::wstring& requestURI);
    web::json::value HEADRequest(const std::wstring& requestURI);
    web::json::value PUTRequest(const std::wstring& requestURI, const web::json::value& jValue);
    web::json::value POSTRequest(const std::wstring& requestURI, const web::json::value& jValue);
    web::json::value DELRequest(const std::wstring& requestURI, const web::json::value& jValue);

    const web::http::client::http_client& GetClient() const;

protected:
    virtual RequestTask MakeRequestTask(
        web::http::client::http_client& client,
        web::http::method httpMethod,
        const std::wstring& requestURI,
        web::json::value const& jValue
    );

    virtual ResponseTask MakeResponseTask(web::http::http_response&& httpResponse);
    virtual web::json::value GetJSON(ResponseTask&& jsonResponse);

private:
    web::json::value MakeRequest(
        web::http::method httpMethod,
        const std::wstring& uri,
        const web::json::value& jValue = web::json::value::null()
    );

    web::http::client::http_client client_;
};

#include "RESTClient.hpp"

} // end interprocess

#endif  // interprocess_rest_client_h