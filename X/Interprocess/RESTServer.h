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

#include "config.h"

namespace interprocess {

template<typename HTTPListener = web::http::experimental::listener::http_listener>
class RESTServer
{
public:
	RESTServer(const std::wstring& baseURI);

	void Listen();

	const HTTPListener& GetListener() const;

protected:
	virtual void AcceptHandler();
	virtual void POSTHandler(const web::json::value& jValue, web::json::value& answer);
	virtual void PUTHandler(const web::json::value& jValue, web::json::value& answer);
	virtual void DELHandler(const web::json::value& jValue, web::json::value& answer);
	virtual void GETHandler(web::http::http_request request);
	virtual void HEADHandler(web::http::http_request request);

	std::map<utility::string_t, utility::string_t>& GetDictionary();

private:
	const std::wstring DELETED_MSG = L"<deleted>";
	const std::wstring FAILED_MSG = L"<failed>";
	const std::wstring NIL_MSG = L"<nil>";
	const std::wstring PUT_MSG = L"<put>";
	const std::wstring UPDATED_MSG = L"<updated>";

	std::wstring WStr(const std::string& s);

	void HandleRequest(
		web::http::http_request request,
		std::function<void(const web::json::value&, web::json::value&)> action
	);

	HTTPListener listener_;
	std::map<utility::string_t, utility::string_t> dictionary_;
};

#include "RESTServer.hpp"

} // end namespace interprocess

#endif // interprocess_rest_server_h
