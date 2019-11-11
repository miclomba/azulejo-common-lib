#ifndef resource_resource_h
#define resource_resource_h

#include <string>
#include <type_traits>
#include <vector>

#include "config.h"

#include <boost/crc.hpp>

#include "IResource.h"
#include "ResourceTypeTraits.hpp"

// Helpful defines
#define ENABLE_IF_CONTAINER_TEMPLATE_DECL \
template<typename U = T, typename std::enable_if_t<std::is_arithmetic<U>::value, int> = 0> 

namespace resource
{
class ResourceSerializer;
class ResourceDeserializer;

template<typename T>
class RESOURCE_DLL_EXPORT Resource : public IResource
{
public:
	friend class ResourceSerializer;
	friend class ResourceDeserializer;

public:
	ENABLE_IF_CONTAINER_TEMPLATE_DECL
	Resource();
	ENABLE_IF_CONTAINER_TEMPLATE_DECL
	Resource(const std::vector<std::vector<T>>& data);
	ENABLE_IF_CONTAINER_TEMPLATE_DECL
	Resource(std::vector<std::vector<T>>&& data);

	Resource(const Resource&);
	Resource& operator=(const Resource&);
	Resource(Resource&&);
	Resource& operator=(Resource&&);

	virtual ~Resource();

	const T* Data() const;
	T* Data();
	const T& Data(const size_t i, const size_t j) const;
	T& Data(const size_t i, const size_t j);

	void Assign(const char* buff, const size_t n) override;

protected:
	int Checksum() const override;

private:
	void LoadInput(const std::vector<std::vector<T>>& data);

	std::vector<T> data_;
};

#include "Resource.hpp"
} // end resource

#undef ENABLE_IF_CONTAINER_TEMPLATE_DECL
#endif // end resource_resource_h

