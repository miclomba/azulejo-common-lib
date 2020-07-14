#ifndef interprocess_http_h
#define interprocess_http_h

#include <string>
#include "Config/filesystem.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

#include "config.h"

namespace interprocess {

class INTERPROCESS_DLL_EXPORT HTTP
{
public:

	void Get(const Path& resultsFile);

};

} // end interprocess

#endif  // interprocess_http_h