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

#define TRACE(msg)            std::wcout << msg
#define TRACE_ACTION(a, k, v) std::wcout << a << L" (" << k << L", " << v << L")\n"

using interprocess::RESTServer;

namespace
{
void display_json_server(json::value const& jvalue, utility::string_t const& prefix)
{
    std::wcout << prefix << jvalue.serialize() << std::endl;
}

void handle_request(
    http_request request,
    std::function<void(json::value const&, json::value&)> action)
{
    auto answer = json::value::object();

    request
        .extract_json()
        .then([&answer, &action](pplx::task<json::value> task) {
        try
        {
            auto const& jvalue = task.get();
            display_json_server(jvalue, L"R: ");

            if (!jvalue.is_null())
            {
                action(jvalue, answer);
            }
        }
        catch (http_exception const& e)
        {
            std::wcout << e.what() << std::endl;
        }
            })
        .wait();


            display_json_server(answer, L"S: ");

            request.reply(status_codes::OK, answer);
}
} // end namespace

RESTServer::RESTServer() :
    listener_(L"http://localhost/restdemo")
{
    listener_.support(methods::GET, [this](http_request request) { handle_get(request); });
    listener_.support(methods::POST, [this](http_request request) { handle_post(request); });
    listener_.support(methods::PUT, [this](http_request request) { handle_put(request); });
    listener_.support(methods::DEL, [this](http_request request) { handle_del(request); });
}

void RESTServer::Listen()
{
    listener_
        .open()
        .then([this]() {TRACE(L"\nstarting to listen\n"); })
        .wait();
}

void RESTServer::handle_get(http_request request)
{
    TRACE(L"\nhandle GET\n");

    auto answer = json::value::object();

    for (auto const& p : dictionary_)
    {
        answer[p.first] = json::value::string(p.second);
    }

    display_json_server(json::value::null(), L"R: ");
    display_json_server(answer, L"S: ");

    request.reply(status_codes::OK, answer);
}

void RESTServer::handle_post(http_request request)
{
    TRACE("\nhandle POST\n");

    handle_request(
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
                    {
                        answer[key] = json::value::string(L"<nil>");
                    }
                    else
                    {
                        answer[pos->first] = json::value::string(pos->second);
                    }
                }
            }
        });
}

void RESTServer::handle_put(http_request request)
{
    TRACE("\nhandle PUT\n");

    handle_request(
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
                    {
                        TRACE_ACTION(L"added", key, value);
                        answer[key] = json::value::string(L"<put>");
                    }
                    else
                    {
                        TRACE_ACTION(L"updated", key, value);
                        answer[key] = json::value::string(L"<updated>");
                    }

                    dictionary_[key] = value;
                }
            }
        });
}

void RESTServer::handle_del(http_request request)
{
    TRACE("\nhandle DEL\n");

    handle_request(
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
                        TRACE_ACTION(L"deleted", pos->first, pos->second);
                        answer[key] = json::value::string(L"<deleted>");
                        keys.insert(key);
                    }
                }
            }

            for (auto const& key : keys)
                dictionary_.erase(key);
        });
}

