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

	void SetColumnSize(const size_t size);
	void SetRowSize(const size_t size);
	size_t GetColumnSize() const;
	size_t GetRowSize() const;

	virtual void Assign(const char* buff, const size_t n) = 0;

protected:
	bool IsDirty() const;
	virtual std::vector<int> Checksum() const = 0;

private:

	size_t M_{ 0 };
	size_t N_{ 0 };

	mutable std::vector<int> checkSum_{-1};
};

} // end resource
#endif // end resource_iresource_h

