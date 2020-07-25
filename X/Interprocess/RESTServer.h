#ifndef interprocess_rest_server_h
#define interprocess_rest_server_h

#include <map>
#include <cpprest/http_listener.h>

#include "config.h"

namespace interprocess {

class INTERPROCESS_DLL_EXPORT RESTServer
{
public:
	RESTServer(const std::wstring& uri);

	void Listen();

protected:
	using http_request = web::http::http_request;

	virtual void GETCallback(http_request request);
	virtual void POSTCallback(http_request request);
	virtual void PUTCallback(http_request request);
	virtual void DELCallback(http_request request);

private:
	using http_listener = web::http::experimental::listener::http_listener;

	http_listener listener_;
	std::map<utility::string_t, utility::string_t> dictionary_;
};

} // end namespace interprocess

#endif // interprocess_rest_server_h
