/**
 * @file Entity.h
 * @brief Declaration of the Entity base class for managing class members.
 */

#ifndef entity_entity_h
#define entity_entity_h

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "config.h"

namespace entity {

/**
* @class Entity
* @brief A base class for managing class members through functions instead of composition.
*/
class ENTITY_DLL_EXPORT Entity
{
public:
    /**
    * @typedef Key
    * @brief Type alias for the key used to identify members.
    */
    using Key = std::string;

    /**
    * @typedef SharedEntity
    * @brief Type alias for a shared pointer to an Entity.
    */
    using SharedEntity = std::shared_ptr<Entity>;

    /**
    * @typedef MemberMap
    * @brief Type alias for a map of keys to shared Entity pointers.
    */
    using MemberMap = std::map<Key, SharedEntity>;

public:
    /**
    * @brief Destructor for the Entity class.
    */
    virtual ~Entity();

    /**
    * @brief Copy constructor.
    * @param other The Entity instance to copy from.
    */
    Entity(const Entity& other);

    /**
    * @brief Copy assignment operator.
    * @param other The Entity instance to copy from.
    * @return Reference to the updated Entity instance.
    */
    Entity& operator=(const Entity& other);

    /**
    * @brief Move constructor.
    * @param other The Entity instance to move from.
    */
    Entity(Entity&& other);

    /**
    * @brief Move assignment operator.
    * @param other The Entity instance to move from.
    * @return Reference to the updated Entity instance.
    */
    Entity& operator=(Entity&& other);

    /**
    * @brief Get the key associated with this Entity.
    * @return The key as a string.
    */
    Key GetKey() const;

    /**
    * @brief Set the key associated with this Entity.
    * @param key The key to set.
    */
    void SetKey(const Key& key);

protected:
    /**
    * @brief Default constructor for the Entity class.
    */
    Entity();

    /**
    * @brief Retrieve a member by its key, casting it to the desired type.
    * @tparam T The type of the member to retrieve.
    * @param key The key of the member to retrieve.
    * @return A shared pointer to the member.
    */
    template<class T>
    SharedEntity& GetAggregatedMember(const Key& key) const;

    /**
    * @brief Retrieve a member by its key (virtual version).
    * @param key The key of the member to retrieve.
    * @return A shared pointer to the member.
    */
    virtual SharedEntity& GetAggregatedMember(const Key& key) const;

    /**
    * @brief Get all aggregated members.
    * @return A reference to the map of keys to shared Entity pointers.
    */
    std::map<Key, SharedEntity>& GetAggregatedMembers() const;

    /**
    * @brief Retrieve the keys of all aggregated members.
    * @tparam T The type of the members (default is Entity).
    * @return A vector of keys.
    */
    template<class T = Entity>
    std::vector<Key> GetAggregatedMemberKeys() const;

    /**
    * @brief Add a new member to the Entity.
    * @param key The key associated with the member.
    */
    void AggregateMember(const Key& key);

    /**
    * @brief Add a new member to the Entity.
    * @tparam T The type of the member.
    * @param sharedObj A shared pointer to the member.
    */
    template<class T>
    void AggregateMember(std::shared_ptr<T> sharedObj);

    /**
    * @brief Remove a member from the Entity by its key.
    * @param key The key of the member to remove.
    */
    void RemoveMember(const Key& key);

    /**
    * @brief Remove a member from the Entity by its shared pointer.
    * @param sharedObj A shared pointer to the member to remove.
    */
    void RemoveMember(SharedEntity sharedObj);

private:
    /**
    * @brief Register a constructor for a specific Entity type.
    * @tparam T The type of the Entity to register.
    * @param key The key associated with the Entity type.
    */
    template<typename T>
    void RegisterEntityConstructor(const Key& key);

    /**
    * @brief Construct a new Entity instance by copying another Entity.
    * @param other The Entity to copy.
    * @return A unique pointer to the newly constructed Entity.
    */
    std::unique_ptr<Entity> ConstructEntity(const Entity& other) const;

    /** @brief The key associated with this Entity. */
    Key key_;

    /** @brief Map of keys to shared Entity pointers. */
    mutable std::map<Key, SharedEntity> membersMap_;

    /** @brief Map of keys to Entity constructors. */
    mutable std::map<Entity::Key, std::function<std::unique_ptr<Entity>(const Entity&)>> keyToEntityConstructorMap_;
};

#include "Entity.hpp"

} // end namespace entity

#endif // entity_entity_h