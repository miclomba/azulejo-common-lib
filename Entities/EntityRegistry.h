/**
 * @file EntityRegistry.h
 * @brief Declaration of the EntityRegistry class for managing entity registration and generation.
 */

#ifndef entity_entityregistry_h
#define entity_entityregistry_h

#include <memory>
#include <stdexcept>
#include <string>
#include <map>
#include <functional>

#include "config.h"
#include "Entity.h"

namespace entity {

/**
* @class EntityRegistry
* @brief A template class for managing the registration and generation of entities.
* @tparam EntityStorageType The type of entities stored in the registry.
*/
template<typename EntityStorageType>
class EntityRegistry
{
public:
    /**
    * @brief Default constructor for the EntityRegistry class.
    */
    EntityRegistry();

    /**
    * @brief Copy constructor.
    * @param other The EntityRegistry instance to copy from.
    */
    EntityRegistry(const EntityRegistry& other);

    /**
    * @brief Copy assignment operator.
    * @param other The EntityRegistry instance to copy from.
    * @return Reference to the updated EntityRegistry instance.
    */
    EntityRegistry& operator=(const EntityRegistry& other);

    /**
    * @brief Move constructor.
    * @param other The EntityRegistry instance to move from.
    */
    EntityRegistry(EntityRegistry&& other);

    /**
    * @brief Move assignment operator.
    * @param other The EntityRegistry instance to move from.
    * @return Reference to the updated EntityRegistry instance.
    */
    EntityRegistry& operator=(EntityRegistry&& other);

    /**
    * @brief Destructor for the EntityRegistry class.
    */
    virtual ~EntityRegistry();

    /**
    * @brief Register a new entity type with the registry.
    * @tparam T The type of the entity to register.
    * @param key The key associated with the entity type.
    */
    template<typename T>
    void RegisterEntity(const Entity::Key& key);

    /**
    * @brief Unregister an entity type by its key.
    * @param key The key of the entity type to unregister.
    */
    void UnregisterEntity(const Entity::Key& key);

    /**
    * @brief Unregister all entity types from the registry.
    */
    void UnregisterAll();

    /**
    * @brief Check if an entity type is registered with the given key.
    * @param key The key to check for.
    * @return True if the key is registered, false otherwise.
    */
    bool HasRegisteredKey(const Entity::Key& key) const;

    /**
    * @brief Generate a new instance of an entity type by its key.
    * @param key The key of the entity type to generate.
    * @return A unique pointer to the newly generated entity.
    */
    std::unique_ptr<EntityStorageType> GenerateEntity(const Entity::Key& key) const;

private:
    /**
    * @brief Map of keys to entity constructors.
    * Each key maps to a function that creates a new instance of the associated entity type.
    */
    mutable std::map<Entity::Key, std::function<std::unique_ptr<EntityStorageType>(void)>> keyToEntityMap_;
};

#include "EntityRegistry.hpp"

} // end namespace entity

#endif // entity_entityregistry_h
