#ifndef resource_iresource_h
#define resource_iresource_h

#include <array>
#include <string>
#include <vector>

#include <boost/crc.hpp>

#include "config.h"
#include "ResourceTypeTraits.hpp"

// Helpful defines
#define ENABLE_IF_CONTAINER_TEMPLATE_DECL \
template<typename U = T, typename std::enable_if_t<is_arithmetic_container<U>::value, int> = 0> 

namespace resource
{
class ResourceSerializer;
class ResourceDeserializer;

template<typename T>
class RESOURCE_DLL_EXPORT IResource 
{
public:
	friend class ResourceSerializer;
	friend class ResourceDeserializer;

public:
	ENABLE_IF_CONTAINER_TEMPLATE_DECL
	IResource(const T& data);
	ENABLE_IF_CONTAINER_TEMPLATE_DECL
	IResource(T&& data);

	virtual ~IResource();

	const T& Data() const;
	T& Data();

protected:
	bool IsDirty() const;

	ENABLE_IF_CONTAINER_TEMPLATE_DECL
	int Checksum() const;

private:
	T data_;
	mutable int checkSum_{ -1 };
};

#include "IResource.hpp"
}

#undef ENABLE_IF_CONTAINER_TEMPLATE_DECL
#endif // end resource_iresource_h

