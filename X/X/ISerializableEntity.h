#ifndef global_iserializable_entity
#define global_iserializable_entity

#include <iostream>
#include <memory>
#include <string>

#include "config.h"
#include "Entity.h"

namespace global
{

class X_DLL_EXPORT ISerializableEntity : public Entity
{
public:
	virtual ~ISerializableEntity();

	virtual std::string Serialize() const = 0;
	virtual void Deserialize(const std::string& serialization) = 0;

	static std::string GetDelimeter();

	friend X_DLL_EXPORT std::ostream& operator<<(std::ostream& to, const ISerializableEntity& from);
	friend X_DLL_EXPORT std::istream& operator>>(std::istream& from, ISerializableEntity& to);
};

} // namespace global

#endif // global_iserializable_entity

