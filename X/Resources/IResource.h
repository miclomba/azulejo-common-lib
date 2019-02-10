#ifndef resource_iresource_h
#define resource_iresource_h

#include <boost/crc.hpp>

#include "config.h"

namespace resource
{

template<class T>
class RESOURCE_DLL_EXPORT IResource 
{
public:
	IResource(const T& data);
	IResource(T&& data);

	virtual ~IResource();

	const T& Data() const;
	T& Data();

	void Save(const std::string& path) const;
	void Load(const std::string& path);

protected:
	bool IsDirty() const;
	int Checksum() const;

	virtual void SaveImpl(const std::string& path) const = 0;
	virtual void LoadImpl(const std::string& path) = 0;

private:
	T data_;
	mutable int checkSum_{ -1 };
};

#include "IResource.hpp"

}
#endif // end resource_iresource_h

