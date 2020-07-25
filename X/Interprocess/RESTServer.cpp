#include "RESTServer.h"

#include <iostream>
#include <map>
#include <set>
#include <string>

#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

using interprocess::RESTServer;

namespace
{
void PrintJSON(json::value const& jvalue, utility::string_t const& prefix)
{
    std::wcout << prefix << jvalue.serialize() << std::endl;
}

void HandleRequest(
    http_request request,
    std::function<void(json::value const&, json::value&)> action
)
{
    auto answer = json::value::object();

    request
        .extract_json()
        .then([&answer, &action](pplx::task<json::value> task) 
        {
            try
            {
                auto const& jvalue = task.get();
                PrintJSON(jvalue, L"R: ");

                if (!jvalue.is_null())
                    action(jvalue, answer);
            }
            catch (http_exception const& e)
            {
                std::wcout << e.what() << std::endl;
            }
        })
        .wait();

    PrintJSON(answer, L"S: ");
    request.reply(status_codes::OK, answer);
}
} // end namespace

RESTServer::RESTServer(const std::wstring& uri) :
    listener_(uri)
{
    listener_.support(methods::GET, [this](http_request request) { GETCallback(request); });
    listener_.support(methods::POST, [this](http_request request) { POSTCallback(request); });
    listener_.support(methods::PUT, [this](http_request request) { PUTCallback(request); });
    listener_.support(methods::DEL, [this](http_request request) { DELCallback(request); });
}

void RESTServer::AcceptCallback()
{

}

void RESTServer::Listen()
{
    listener_
        .open()
        .then([this]() { AcceptCallback(); })
        .wait();
}

void RESTServer::GETCallback(http_request request)
{
    web::json::value answer = web::json::value::object();

    for (const std::pair<utility::string_t, utility::string_t>& p : dictionary_)
        answer[p.first] = json::value::string(p.second);

    request.reply(status_codes::OK, answer);
}

void RESTServer::POSTCallback(http_request request)
{
    HandleRequest(
        request,
        [this](json::value const& jvalue, json::value& answer)
        {
            for (auto const& e : jvalue.as_array())
            {
                if (e.is_string())
                {
                    auto key = e.as_string();
                    auto pos = dictionary_.find(key);

                    if (pos == dictionary_.end())
                        answer[key] = json::value::string(L"<nil>");
                    else
                        answer[pos->first] = json::value::string(pos->second);
                }
            }
        }
    );
}

void RESTServer::PUTCallback(http_request request)
{
    HandleRequest(
        request,
        [this](json::value const& jvalue, json::value& answer)
        {
            for (auto const& e : jvalue.as_object())
            {
                if (e.second.is_string())
                {
                    auto key = e.first;
                    auto value = e.second.as_string();

                    if (dictionary_.find(key) == dictionary_.end())
                        answer[key] = json::value::string(L"<put>");
                    else
                        answer[key] = json::value::string(L"<updated>");

                    dictionary_[key] = value;
                }
            }
        }
    );
}

void RESTServer::DELCallback(http_request request)
{
    HandleRequest(
        request,
        [this](json::value const& jvalue, json::value& answer)
        {
            std::set<utility::string_t> keys;
            for (auto const& e : jvalue.as_array())
            {
                if (e.is_string())
                {
                    auto key = e.as_string();

                    auto pos = dictionary_.find(key);
                    if (pos == dictionary_.end())
                    {
                        answer[key] = json::value::string(L"<failed>");
                    }
                    else
                    {
                        answer[key] = json::value::string(L"<deleted>");
                        keys.insert(key);
                    }
                }
            }

            for (auto const& key : keys)
                dictionary_.erase(key);
        }
    );
}

