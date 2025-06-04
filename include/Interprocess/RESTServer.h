/**
 * @file RESTServer.h
 * @brief Declaration of the RESTServer class template for managing a RESTful web server.
 */

#ifndef interprocess_rest_server_h
#define interprocess_rest_server_h

#include <functional>
#include <codecvt>
#include <locale>
#include <map>
#include <set>
#include <string>
#include <utility>

#include <cpprest/details/basic_types.h>
#include <cpprest/http_listener.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>

#include "Interprocess/config.h"

namespace interprocess
{

    /**
     * @class RESTServer
     * @brief A class template for managing a RESTful web server using the cpprest SDK.
     *
     * @tparam HTTPListener The type of the HTTP listener (default: web::http::experimental::listener::http_listener).
     */
    template <typename HTTPListener = web::http::experimental::listener::http_listener>
    class RESTServer
    {
    public:
        /**
         * @brief Constructor for the RESTServer class.
         * @param baseURI The base URI for the REST server.
         */
        RESTServer(const std::wstring &baseURI);

        /**
         * @brief Start listening for incoming HTTP requests.
         */
        void Listen();

        /**
         * @brief Get the HTTP listener used by the REST server.
         * @return A constant reference to the HTTP listener.
         */
        const HTTPListener &GetListener() const;

    protected:
        /**
         * @brief Handle a new connection.
         *
         * This method can be overridden in derived classes to provide custom behavior.
         */
        virtual void AcceptHandler();

        /**
         * @brief Handle HTTP POST requests.
         * @param jValue The JSON payload from the request.
         * @param answer The JSON response to be sent.
         */
        virtual void POSTHandler(const web::json::value &jValue, web::json::value &answer);

        /**
         * @brief Handle HTTP PUT requests.
         * @param jValue The JSON payload from the request.
         * @param answer The JSON response to be sent.
         */
        virtual void PUTHandler(const web::json::value &jValue, web::json::value &answer);

        /**
         * @brief Handle HTTP DELETE requests.
         * @param jValue The JSON payload from the request.
         * @param answer The JSON response to be sent.
         */
        virtual void DELHandler(const web::json::value &jValue, web::json::value &answer);

        /**
         * @brief Handle HTTP GET requests.
         * @param request The HTTP request object.
         */
        virtual void GETHandler(web::http::http_request request);

        /**
         * @brief Handle HTTP HEAD requests.
         * @param request The HTTP request object.
         */
        virtual void HEADHandler(web::http::http_request request);

        /**
         * @brief Get the dictionary of key-value pairs managed by the server.
         * @return A reference to the dictionary.
         */
        std::map<utility::string_t, utility::string_t> &GetDictionary();

    private:
        /**
         * @brief Convert a standard string to a wide string.
         * @param s The standard string to convert.
         * @return The converted wide string.
         */
        std::wstring WStr(const std::string &s);

        /**
         * @brief Handle an HTTP request.
         * @param request The HTTP request object.
         * @param action The action to perform on the request payload and response.
         */
        void HandleRequest(
            web::http::http_request request,
            std::function<void(const web::json::value &, web::json::value &)> action);

        /** @brief HTTP listener for handling incoming requests. */
        HTTPListener listener_;

        /** @brief Dictionary for managing key-value pairs. */
        std::map<utility::string_t, utility::string_t> dictionary_;

        /** @brief Predefined message indicating a deleted state. */
        const std::wstring DELETED_MSG = L"<deleted>";

        /** @brief Predefined message indicating a failed state. */
        const std::wstring FAILED_MSG = L"<failed>";

        /** @brief Predefined message indicating a nil state. */
        const std::wstring NIL_MSG = L"<nil>";

        /** @brief Predefined message indicating a put operation. */
        const std::wstring PUT_MSG = L"<put>";

        /** @brief Predefined message indicating an updated state. */
        const std::wstring UPDATED_MSG = L"<updated>";
    };

#include "Interprocess/RESTServer.hpp"

} // end namespace interprocess

#endif // interprocess_rest_server_h
