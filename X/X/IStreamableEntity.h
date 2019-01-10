#ifndef global_istreamable_entity
#define global_istreamable_entity

#include <iostream>
#include <memory>
#include <string>

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT IStreamableEntity : public Entity
{
public:
	virtual ~IStreamableEntity();

	virtual std::string ToString() const = 0;
	virtual void FromString(const std::string& serialization) = 0;

	static std::string GetDelimeter();

	friend X_DLL_EXPORT std::ostream& operator<<(std::ostream& to, const IStreamableEntity& from);
	friend X_DLL_EXPORT std::istream& operator>>(std::istream& from, IStreamableEntity& to);
};

} // namespace global

#endif // global_istreamable_entity

