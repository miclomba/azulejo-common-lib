
#define TEMPLATE_T template<typename HTTPListener>
#define RESTServer_t RESTServer<HTTPListener>

//using namespace utility;                    // Common utilities like string conversions
//using namespace web;                        // Common features like URIs and json.
//using namespace web::http;                  // Common HTTP functionality
//using namespace web::http::client;          // HTTP client features
//using namespace concurrency::streams;       // Asynchronous streams

TEMPLATE_T
std::wstring RESTServer_t::WStr(const std::string& s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(s);
}

TEMPLATE_T
void RESTServer_t::HandleRequest(
    web::http::http_request request,
    std::function<void(const web::json::value&, web::json::value&)> action
)
{
    web::json::value answer = web::json::value::object();
    web::http::status_code code = web::http::status_codes::OK;

    request
        .extract_json()
        .then([this, &code, &answer, &action](pplx::task<web::json::value> task) 
        {
            try
            {
                const web::json::value& jValue = task.get();

                if (!jValue.is_null())
                    action(jValue, answer);
            }
            catch (const web::http::http_exception& e)
            {
                code = web::http::status_codes::BadRequest;
                answer = web::json::value(this->WStr("ERROR: RESTServer: ") + WStr(e.what()));
            }
        })
        .wait();

    request.reply(code, answer);
}

TEMPLATE_T
RESTServer_t::RESTServer(const std::wstring& baseURI) :
    listener_(baseURI)
{
    listener_.support(web::http::methods::GET, [this](web::http::http_request request) { 
        GETHandler(request); 
    });
    listener_.support(web::http::methods::HEAD, [this](web::http::http_request request) {
        HEADHandler(request);
    });
    listener_.support(web::http::methods::POST, [this](web::http::http_request request) {     
        HandleRequest(
            request,
            [this](const web::json::value& jValue, web::json::value& answer) { POSTHandler(jValue, answer); }
        ); 
    });
    listener_.support(web::http::methods::PUT, [this](web::http::http_request request) {
        HandleRequest(
            request,
            [this](const web::json::value& jValue, web::json::value& answer) { PUTHandler(jValue, answer); }
        );
    });
    listener_.support(web::http::methods::DEL, [this](web::http::http_request request) {
        HandleRequest(
            request,
            [this](const web::json::value& jValue, web::json::value& answer) { DELHandler(jValue, answer); }
        );
    });
}

TEMPLATE_T
void RESTServer_t::Listen()
{
    listener_
        .open()
        .then([this]() { AcceptHandler(); })
        .wait();
}

TEMPLATE_T
const HTTPListener& RESTServer_t::GetListener() const
{
    return listener_;
}

TEMPLATE_T
void RESTServer_t::GETHandler(web::http::http_request request)
{
    web::json::value answer = web::json::value::object();

    for (const std::pair<utility::string_t, utility::string_t>& p : dictionary_)
        answer[p.first] = web::json::value::string(p.second);

    request.reply(web::http::status_codes::OK, answer);
}

TEMPLATE_T
void RESTServer_t::HEADHandler(web::http::http_request request)
{
    web::json::value answer = web::json::value::object();

    request.reply(web::http::status_codes::OK, answer);
}

TEMPLATE_T
void RESTServer_t::AcceptHandler()
{

}

TEMPLATE_T
void RESTServer_t::POSTHandler(const web::json::value& jValue, web::json::value& answer)
{
    for (const web::json::value& e : jValue.as_array())
    {
        if (e.is_string())
        {
            const utility::string_t& key = e.as_string();
            auto pos = dictionary_.find(key);

            if (pos == dictionary_.end())
                answer[key] = web::json::value::string(NIL_MSG);
            else
                answer[pos->first] = web::json::value::string(pos->second);
        }
    }
}

TEMPLATE_T
void RESTServer_t::PUTHandler(const web::json::value& jValue, web::json::value& answer)
{
    for (const std::pair<utility::string_t, web::json::value>& e : jValue.as_object())
    {
        if (e.second.is_string())
        {
            utility::string_t key = e.first;
            utility::string_t value = e.second.as_string();

            if (dictionary_.find(key) == dictionary_.end())
                answer[key] = web::json::value::string(PUT_MSG);
            else
                answer[key] = web::json::value::string(UPDATED_MSG);

            dictionary_[key] = value;
        }
    }
}

TEMPLATE_T
void RESTServer_t::DELHandler(const web::json::value& jValue, web::json::value& answer)
{
    std::set<utility::string_t> keys;
    for (const web::json::value& e : jValue.as_array())
    {
        if (e.is_string())
        {
            utility::string_t key = e.as_string();

            auto pos = dictionary_.find(key);
            if (pos == dictionary_.end())
            {
                answer[key] = web::json::value::string(FAILED_MSG);
            }
            else
            {
                answer[key] = web::json::value::string(DELETED_MSG);
                keys.insert(key);
            }
        }
    }

    for (const utility::string_t& key : keys)
        dictionary_.erase(key);
}

TEMPLATE_T
std::map<utility::string_t, utility::string_t>& RESTServer_t::GetDictionary()
{
    return dictionary_;
}

#undef RESTServer_t
#undef TEMPLATE_T

