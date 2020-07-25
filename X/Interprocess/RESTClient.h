#ifndef interprocess_rest_client_h
#define interprocess_rest_client_h

#include <string>
#include "Config/filesystem.h"

#include <cpprest/details/basic_types.h>
#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>
//#include <cpprest/filestream.h>

#include "config.h"

namespace interprocess {

class INTERPROCESS_DLL_EXPORT RESTClient
{
public:
    RESTClient(const std::wstring& uri);

    web::json::value MakeRequest(
        web::http::method mtd, 
        const std::wstring& uri, 
        const web::json::value& jValue
    );

private:
    web::http::client::http_client client_;
};

} // end interprocess

#endif  // interprocess_rest_client_h