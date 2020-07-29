
//using namespace utility;                    // Common utilities like string conversions
//using namespace web;                        // Common features like URIs and json.
//using namespace web::http;                  // Common HTTP functionality
//using namespace web::http::client;          // HTTP client features
//using namespace concurrency::streams;       // Asynchronous streams

#define TEMPLATE_T template<typename RequestTask, typename ResponseTask, typename ResultTask>
#define RESTClient_t RESTClient<RequestTask, ResponseTask, ResultTask>

TEMPLATE_T
RESTClient_t::RESTClient(const std::wstring& baseURI) :
    client_(baseURI)
{
}

TEMPLATE_T
const web::http::client::http_client& RESTClient_t::GetClient() const
{
    return client_;
}

TEMPLATE_T
web::json::value RESTClient_t::GETRequest(const std::wstring& requestURI)
{
    return RESTClient::MakeRequest(web::http::methods::GET, requestURI);
}

TEMPLATE_T
web::json::value RESTClient_t::HEADRequest(const std::wstring& requestURI)
{
    return RESTClient::MakeRequest(web::http::methods::HEAD, requestURI);
}

TEMPLATE_T
web::json::value RESTClient_t::PUTRequest(const std::wstring& requestURI, const web::json::value& jValue)
{
    return RESTClient::MakeRequest(web::http::methods::PUT, requestURI, jValue);
}

TEMPLATE_T
web::json::value RESTClient_t::POSTRequest(const std::wstring& requestURI, const web::json::value& jValue)
{
    return RESTClient::MakeRequest(web::http::methods::POST, requestURI, jValue);
}

TEMPLATE_T
web::json::value RESTClient_t::DELRequest(const std::wstring& requestURI, const web::json::value& jValue)
{
    return RESTClient::MakeRequest(web::http::methods::DEL, requestURI, jValue);
}

TEMPLATE_T
RequestTask RESTClient_t::MakeRequestTask(
    web::http::client::http_client& client,
    web::http::method httpMethod,
    const std::wstring& requestURI,
    web::json::value const& jValue)
{
    return (httpMethod == web::http::methods::GET || httpMethod == web::http::methods::HEAD) ?
        client.request(httpMethod, requestURI) :
        client.request(httpMethod, requestURI, jValue);
}

TEMPLATE_T
ResponseTask RESTClient_t::MakeResponseTask(web::http::http_response&& httpResponse)
{
    if (httpResponse.status_code() == web::http::status_codes::OK)
        return httpResponse.extract_json();
    return pplx::task_from_result(web::json::value());
}

TEMPLATE_T
web::json::value RESTClient_t::GetJSON(ResponseTask&& jsonResponse)
{
    try
    {
        return std::move(jsonResponse.get());
    }
    catch (const web::http::http_exception& e)
    {
        auto WStr = [](const std::string& s) -> std::wstring
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            return conv.from_bytes(s);
        };

        return web::json::value(WStr("ERROR: RESTClient: ") + WStr(e.what()));
    }
}

TEMPLATE_T
web::json::value RESTClient_t::MakeRequest(web::http::method httpMethod, const std::wstring& requestURI, const web::json::value& jValue)
{
    web::json::value response;

    RequestTask requestTask = std::move(MakeRequestTask(client_, httpMethod, requestURI, jValue));
    ResponseTask responseTask = std::move(requestTask.then([this](web::http::http_response httpResponse)
        {
            return MakeResponseTask(std::move(httpResponse));
        }));

    ResultTask resultTask = std::move(responseTask.then([this,&response](ResponseTask jsonResponse)
        {
            response = std::move(GetJSON(std::move(jsonResponse)));
        }));

    resultTask.wait();

    return response;
}

#undef RESTClient_t
#undef TEMPLATE_T

