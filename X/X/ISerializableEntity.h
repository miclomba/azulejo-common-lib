#ifndef global_iserializable_entity
#define global_iserializable_entity

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

	friend X_DLL_EXPORT std::ostream& operator<<(std::ostream& to, const ISerializableEntity& from);

protected:

	void AggregateMember(const std::string& key, std::shared_ptr<Entity> entity) override;

};

} // namespace global

#endif // global_iserializable_entity

