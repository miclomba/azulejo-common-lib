#ifndef resource_resource_2d_h
#define resource_resource_2d_h

#include <string>
#include <type_traits>
#include <vector>

#include "config.h"

#include <boost/crc.hpp>

#include "Resource.h"

namespace resource
{
template<typename T>
class RESOURCE_DLL_EXPORT Resource2D : public Resource<T>
{
public:
	Resource2D();
	Resource2D(const std::vector<std::vector<T>>& data);
	Resource2D(std::vector<std::vector<T>>&& data);

	Resource2D(const Resource2D&);
	Resource2D& operator=(const Resource2D&);
	Resource2D(Resource2D&&);
	Resource2D& operator=(Resource2D&&);

	virtual ~Resource2D();

	const T& GetData(const size_t i, const size_t j) const;
	T& GetData(const size_t i, const size_t j);

	void Assign(const char* buff, const size_t n) override;

private:
	void LoadInput(const std::vector<std::vector<T>>& data);
};

#include "Resource2D.hpp"
} // end resource

#endif // end resource_resource_2d_h

