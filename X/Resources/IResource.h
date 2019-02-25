#ifndef resource_iresource_h
#define resource_iresource_h

#include "config.h"

namespace resource
{
class ResourceSerializer;
class ResourceDeserializer;

class RESOURCE_DLL_EXPORT IResource
{
public:
	friend class ResourceSerializer;
	friend class ResourceDeserializer;

private:
	virtual void Assign(const char* buff, const size_t n) = 0;
};
}

#endif // end resource_iresource_h

