/**
 * @file ISerializableEntity.h
 * @brief Declaration of the ISerializableEntity interface for serializable entities.
 */

#ifndef filesystem_adapters_iserializableentity_h
#define filesystem_adapters_iserializableentity_h

#include <map>
#include <memory>
#include <string>

#include "config.h"
#include <boost/property_tree/ptree.hpp>
#include "Entities/Entity.h"

namespace filesystem_adapters {

class EntityDeserializer;
class EntitySerializer;

/**
* @class ISerializableEntity
* @brief An interface for entities that can be serialized and deserialized.
*
* Inherits from the `entity::Entity` class and provides methods for saving and loading entity data.
*/
class FILESYSTEM_ADAPTERS_DLL_EXPORT ISerializableEntity : public virtual entity::Entity
{
public:
    /**
    * @brief Allows EntitySerializer and EntityDeserializer access to private and protected members.
    */
    friend class EntitySerializer;
    friend class EntityDeserializer;

public:
    /**
    * @brief Default constructor for the ISerializableEntity class.
    */
    ISerializableEntity();

    /**
    * @brief Destructor for the ISerializableEntity class.
    */
    virtual ~ISerializableEntity();

    /**
    * @brief Copy constructor.
    * @param other The ISerializableEntity instance to copy from.
    */
    ISerializableEntity(const ISerializableEntity& other);

    /**
    * @brief Copy assignment operator.
    * @param other The ISerializableEntity instance to copy from.
    * @return Reference to the updated ISerializableEntity instance.
    */
    ISerializableEntity& operator=(const ISerializableEntity& other);

    /**
    * @brief Move constructor.
    * @param other The ISerializableEntity instance to move from.
    */
    ISerializableEntity(ISerializableEntity&& other);

    /**
    * @brief Move assignment operator.
    * @param other The ISerializableEntity instance to move from.
    * @return Reference to the updated ISerializableEntity instance.
    */
    ISerializableEntity& operator=(ISerializableEntity&& other);

    /**
    * @brief Save the entity data to a property tree.
    * @param tree The property tree to save data into.
    * @param path The path within the tree where the data should be saved.
    */
    virtual void Save(boost::property_tree::ptree& tree, const std::string& path) const = 0;

    /**
    * @brief Load the entity data from a property tree.
    * @param tree The property tree containing the data.
    * @param path The path within the tree where the data is stored.
    */
    virtual void Load(boost::property_tree::ptree& tree, const std::string& path) = 0;

protected:
    /**
    * @brief Retrieve an aggregated member by its key.
    * @param key The key of the aggregated member to retrieve.
    * @return A shared pointer to the aggregated member.
    */
    SharedEntity& GetAggregatedMember(const Key& key) const override;
};

} // end namespace filesystem_adapters

#endif // filesystem_adapters_iserializableentity_h