#ifndef global_resource
#define global_resource

#include "config.h"

namespace global
{

template<class T>
class X_DLL_EXPORT Resource 
{
public:
    virtual ~Resource() = default;

protected:
	const T& Data() const;
	T& Data();

private:
	T data_;
};

#include "Resource.hpp"

}
#endif

