#ifndef interprocess_rest_server_h
#define interprocess_rest_server_h

#include <map>
#include <string>

#include <cpprest/details/basic_types.h>
#include <cpprest/http_listener.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>

#include "config.h"

namespace interprocess {

class INTERPROCESS_DLL_EXPORT RESTServer
{
public:
	RESTServer(const std::wstring& serverURI);

	void Listen();

protected:
	virtual void AcceptHandler();
	virtual void POSTHandler(const web::json::value& jValue, web::json::value& answer);
	virtual void PUTHandler(const web::json::value& jValue, web::json::value& answer);
	virtual void DELHandler(const web::json::value& jValue, web::json::value& answer);

private:
	void GETHandler(web::http::http_request request);
	void HEADHandler(web::http::http_request request);

	web::http::experimental::listener::http_listener listener_;
	std::map<utility::string_t, utility::string_t> dictionary_;
};

} // end namespace interprocess

#endif // interprocess_rest_server_h
