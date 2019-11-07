#ifndef resource_iresource_h
#define resource_iresource_h

#include <vector>

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

	IResource();
	virtual ~IResource();

	IResource(const IResource&);
	IResource& operator=(const IResource&);
	IResource(IResource&&);
	IResource& operator=(IResource&&);

	virtual void Assign(const char* buff, const size_t n) = 0;

protected:
	bool IsDirty() const;
	virtual std::vector<int> Checksum() const = 0;

private:
	mutable std::vector<int> checkSum_;
};

} // end resource
#endif // end resource_iresource_h

