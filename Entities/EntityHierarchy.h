/**
 * @file EntityHierarchy.h
 * @brief Declaration of the EntityHierarchy class for managing the serialization structure of runtime objects.
 */

#ifndef entity_entityhierarchy_h
#define entity_entityhierarchy_h

#include <string>
#include "Config/filesystem.h"
#include "config.h"
#include <boost/property_tree/ptree.hpp>

namespace entity {

/**
* @class EntityHierarchy
* @brief A class that manages the serialization structure of runtime objects.
*
* Provides methods to load, manage, and query the serialization structure for runtime entities.
*/
class ENTITY_DLL_EXPORT EntityHierarchy
{
public:
    /**
    * @brief Default constructor for the EntityHierarchy class.
    */
    EntityHierarchy();

    /**
    * @brief Copy constructor.
    * @param other The EntityHierarchy instance to copy from.
    */
    EntityHierarchy(const EntityHierarchy& other);

    /**
    * @brief Copy assignment operator.
    * @param other The EntityHierarchy instance to copy from.
    * @return Reference to the updated EntityHierarchy instance.
    */
    EntityHierarchy& operator=(const EntityHierarchy& other);

    /**
    * @brief Move constructor.
    * @param other The EntityHierarchy instance to move from.
    */
    EntityHierarchy(EntityHierarchy&& other);

    /**
    * @brief Move assignment operator.
    * @param other The EntityHierarchy instance to move from.
    * @return Reference to the updated EntityHierarchy instance.
    */
    EntityHierarchy& operator=(EntityHierarchy&& other);

    /**
    * @brief Destructor for the EntityHierarchy class.
    */
    virtual ~EntityHierarchy();

    /**
    * @brief Load the serialization structure from a JSON file.
    * @param pathToJSON The path to the JSON file containing the serialization structure.
    */
    void LoadSerializationStructure(const Path& pathToJSON);

    /**
    * @brief Check if the serialization structure is loaded.
    * @return True if the serialization structure is loaded, false otherwise.
    */
    bool HasSerializationStructure() const;

    /**
    * @brief Get the loaded serialization structure.
    * @return A reference to the property tree representing the serialization structure.
    */
    boost::property_tree::ptree& GetSerializationStructure();

    /**
    * @brief Set the path to the serialization structure.
    * @param pathToJSON The path to the JSON file containing the serialization structure.
    */
    void SetSerializationPath(const Path& pathToJSON);

    /**
    * @brief Get the path to the serialization structure.
    * @return The path to the JSON file containing the serialization structure.
    */
    Path GetSerializationPath() const;

private:
    /** @brief Path to the JSON file containing the serialization structure. */
    Path serializationPath_;

    /** @brief Property tree representing the serialization structure. */
    boost::property_tree::ptree serializationStructure_;
};

} // end namespace entity

#endif // entity_entityhierarchy_h
