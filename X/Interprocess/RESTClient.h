#ifndef interprocess_http_h
#define interprocess_http_h

#include <string>
#include "Config/filesystem.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

#include "config.h"

namespace interprocess {

class INTERPROCESS_DLL_EXPORT RESTClient
{
public:
    RESTClient(const std::wstring& uri);

    void MakeRequest(web::http::method mtd, const std::wstring& uri, const web::json::value& jvalue);

private:
    web::http::client::http_client client_;

};

} // end interprocess

#endif  // interprocess_http_h