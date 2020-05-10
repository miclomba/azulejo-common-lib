#ifndef resource_iresource_h
#define resource_iresource_h

#include <vector>

#include "config.h"

namespace resource
{

class RESOURCE_DLL_EXPORT IResource
{
public:
	IResource();
	virtual ~IResource();

	IResource(const IResource&);
	IResource& operator=(const IResource&);
	IResource(IResource&&);
	IResource& operator=(IResource&&);

	size_t GetColumnSize() const;
	size_t GetRowSize() const;

	virtual size_t GetElementSize() const = 0;
	virtual void* Data() = 0;
	virtual const void* Data() const = 0;
	virtual void Assign(const char* buff, const size_t n) = 0;

protected:
	void SetColumnSize(const size_t size);
	void SetRowSize(const size_t size);

private:
	size_t M_{ 0 };
	size_t N_{ 0 };
};

} // end resource
#endif // end resource_iresource_h

