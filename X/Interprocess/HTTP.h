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

	std::wstring GetJSON(const Path& uri);
	void Get(const Path& uri, const Path& resultPath);

};

} // end interprocess

#endif  // interprocess_http_h