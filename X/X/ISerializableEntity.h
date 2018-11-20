#ifndef global_iserializable_entity
#define global_iserializable_entity

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

	friend X_DLL_EXPORT std::ostream& operator<<(std::ostream& to, const ISerializableEntity& from);
};

} // namespace global

#endif // global_iserializable_entity

