#ifndef entity_entityregistry_h
#define entity_entityregistry_h

#include <memory>
#include <stdexcept>
#include <string>

#include "config.h"

#include "Entity.h"

namespace entity {

template<typename EntityStorageType>
class EntityRegistry
{
public:
	EntityRegistry();
	EntityRegistry(const EntityRegistry&);
	EntityRegistry& operator=(const EntityRegistry&);
	EntityRegistry(EntityRegistry&&);
	EntityRegistry& operator=(EntityRegistry&&);

	virtual ~EntityRegistry();

	template<typename T>
	void RegisterEntity(const Entity::Key& key);
	void UnregisterEntity(const Entity::Key& key);
	void UnregisterAll();
	bool HasRegisteredKey(const Entity::Key& key) const;

	std::unique_ptr<EntityStorageType> GenerateEntity(const Entity::Key& key) const;

private:
	mutable std::map<Entity::Key, std::function<std::unique_ptr<EntityStorageType>(void)>> keyToEntityMap_;
};

#include "EntityRegistry.hpp"

} // end namespace entity
#endif // entity_entityregistry_h

