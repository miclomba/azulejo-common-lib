/**
 * @file EntitySerializer.h
 * @brief Declaration of the EntitySerializer class for serializing entities.
 */

#ifndef filesystem_adapters_entityserializer_h
#define filesystem_adapters_entityserializer_h

#include <string>
#include "Config/filesystem.h"
#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "ISerializableEntity.h"

namespace filesystem_adapters {

/**
* @class EntitySerializer
* @brief A singleton class responsible for serializing entities.
*
* Provides methods to serialize entities and manage their hierarchy.
*/
class FILESYSTEM_ADAPTERS_DLL_EXPORT EntitySerializer
{
public:
    /**
    * @brief Destructor for the EntitySerializer class.
    */
    virtual ~EntitySerializer();

    /**
    * @brief Get the singleton instance of the EntitySerializer.
    * @return Pointer to the EntitySerializer instance.
    */
    static EntitySerializer* GetInstance();

    /**
    * @brief Reset the singleton instance of the EntitySerializer.
    */
    static void ResetInstance();

    /**
    * @brief Serialize an entity.
    * @param entity The serializable entity to serialize.
    */
    void Serialize(const ISerializableEntity& entity);

    /**
    * @brief Get the hierarchy of serialized entities.
    * @return Reference to the EntityHierarchy.
    */
    entity::EntityHierarchy& GetHierarchy();

private:
    /**
    * @brief Default constructor for the EntitySerializer class.
    *
    * Private to enforce the singleton pattern.
    */
    EntitySerializer();

    /**
    * @brief Deleted copy constructor to prevent copying.
    */
    EntitySerializer(const EntitySerializer&) = delete;

    /**
    * @brief Deleted copy assignment operator to prevent copying.
    * @return Reference to the updated instance (not used).
    */
    EntitySerializer& operator=(const EntitySerializer&) = delete;

    /**
    * @brief Deleted move constructor to prevent moving.
    */
    EntitySerializer(EntitySerializer&&) = delete;

    /**
    * @brief Deleted move assignment operator to prevent moving.
    * @return Reference to the updated instance (not used).
    */
    EntitySerializer& operator=(EntitySerializer&&) = delete;

    /**
    * @brief Serialize an entity with an optional parent key.
    * @param entity The entity to serialize.
    * @param parentKey The key of the parent entity (default is an empty string).
    */
    void SerializeWithParentKey(const ISerializableEntity& entity, const entity::Entity::Key& parentKey = "");

    /** @brief Pointer to the singleton instance of the EntitySerializer. */
    static EntitySerializer* instance_;

    /** @brief Hierarchy of serialized entities. */
    entity::EntityHierarchy hierarchy_;
};

} // end namespace filesystem_adapters

#endif // filesystem_adapters_entityserializer_h
