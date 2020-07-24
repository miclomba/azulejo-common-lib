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

    RESTClient();

    void make_request(web::http::method mtd, web::json::value const& jvalue);

private:
    web::http::client::http_client client_;

};

} // end interprocess

#endif  // interprocess_http_h