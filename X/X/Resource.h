#ifndef dataresource_resource_h
#define dataresource_resource_h

#include <boost/crc.hpp>

#include "config.h"

namespace dataresource
{

template<class T>
class ENTITY_DLL_EXPORT Resource 
{
public:
	virtual ~Resource();

	bool IsDirty() const;

	const T& Data() const;
	T& Data();

protected:
	int Checksum() const;

private:
	T data_;
	mutable int checkSum_{ -1 };
};

#include "Resource.hpp"

}
#endif

