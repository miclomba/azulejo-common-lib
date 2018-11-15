#ifndef global_entity
#define global_entity

#include "config.h"

namespace global
{

template<class T>
class X_DLL_EXPORT Entity
{
public:
    virtual ~Entity() = default;

protected:
	const T& Data() const;
	T& Data();

private:
	T data_;
};

#include "Entity.hpp"

}
#endif

