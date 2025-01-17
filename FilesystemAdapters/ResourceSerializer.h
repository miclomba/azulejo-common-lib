/**
 * @file ResourceSerializer.h
 * @brief Declaration of the ResourceSerializer class for serializing and deserializing resources.
 */

#ifndef filesystem_adapters_resourceserializer_h
#define filesystem_adapters_resourceserializer_h

#include <fstream>
#include <stdexcept>
#include <string>
#include "Config/filesystem.h"

#include "config.h"
#include "ISerializableResource.h"

namespace filesystem_adapters {

/**
* @class ResourceSerializer
* @brief A singleton class responsible for serializing and deserializing resources.
*/
class FILESYSTEM_ADAPTERS_DLL_EXPORT ResourceSerializer
{
public:
    /**
    * @brief Destructor for the ResourceSerializer class.
    */
    virtual ~ResourceSerializer();

    /**
    * @brief Get the singleton instance of the ResourceSerializer.
    * @return Pointer to the ResourceSerializer instance.
    */
    static ResourceSerializer* GetInstance();

    /**
    * @brief Reset the singleton instance of the ResourceSerializer.
    */
    static void ResetInstance();

    /**
    * @brief Set the path for resource serialization.
    * @param binaryFilePath The file path where resources will be serialized.
    */
    void SetSerializationPath(const std::string& binaryFilePath);

    /**
    * @brief Get the path for resource serialization.
    * @return The file path where resources are serialized.
    */
    std::string GetSerializationPath() const;

    /**
    * @brief Serialize a resource with the specified key.
    * @param resource The resource to serialize.
    * @param key The key associated with the resource.
    */
    void Serialize(const ISerializableResource& resource, const std::string& key);

    /**
    * @brief Deserialize a resource with the specified key.
    * @param key The key associated with the resource to deserialize.
    */
    void Unserialize(const std::string& key);

private:
    /**
    * @brief Default constructor for the ResourceSerializer class.
    *
    * Private to enforce the singleton pattern.
    */
    ResourceSerializer();

    /**
    * @brief Deleted copy constructor to prevent copying.
    */
    ResourceSerializer(const ResourceSerializer&) = delete;

    /**
    * @brief Deleted copy assignment operator to prevent copying.
    * @return Reference to the updated instance (not used).
    */
    ResourceSerializer& operator=(const ResourceSerializer&) = delete;

    /**
    * @brief Deleted move constructor to prevent moving.
    */
    ResourceSerializer(ResourceSerializer&&) = delete;

    /**
    * @brief Deleted move assignment operator to prevent moving.
    * @return Reference to the updated instance (not used).
    */
    ResourceSerializer& operator=(ResourceSerializer&&) = delete;

    /** @brief Pointer to the singleton instance of the ResourceSerializer. */
    static ResourceSerializer* instance_;

    /** @brief Path for resource serialization. */
    Path serializationPath_;
};

} // end namespace filesystem_adapters

#endif // filesystem_adapters_resourceserializer_h
