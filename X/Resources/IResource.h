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

	void SetColumnSize(const size_t size);
	void SetRowSize(const size_t size);
	size_t GetColumnSize() const;
	size_t GetRowSize() const;

	virtual void* Data() = 0;
	virtual const void* Data() const = 0;
	virtual size_t GetElementSize() const = 0;
	virtual void Assign(const char* buff, const size_t n) = 0;

	bool IsDirty() const;
protected:
	virtual int Checksum() const = 0;

private:

	size_t M_{ 0 };
	size_t N_{ 0 };

	mutable int checkSum_{-1};
};

} // end resource
#endif // end resource_iresource_h

