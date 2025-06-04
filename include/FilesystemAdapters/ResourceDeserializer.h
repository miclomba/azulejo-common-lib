/**
 * @file ResourceDeserializer.h
 * @brief Declaration of the ResourceDeserializer class for deserializing resources.
 */

#ifndef filesystem_adapters_resourcedeserializer_h
#define filesystem_adapters_resourcedeserializer_h

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include "Config/filesystem.h"

#include "FilesystemAdapters/config.h"
#include "FilesystemAdapters/ISerializableResource.h"

namespace filesystem_adapters
{

    /**
     * @class ResourceDeserializer
     * @brief A singleton class responsible for deserializing resources.
     *
     * Provides methods for managing resource registrations and deserializing resources based on keys.
     */
    class FILESYSTEM_ADAPTERS_DLL_EXPORT ResourceDeserializer
    {
    public:
        /**
         * @brief Destructor for the ResourceDeserializer class.
         */
        virtual ~ResourceDeserializer();

        /**
         * @brief Get the singleton instance of the ResourceDeserializer.
         * @return Pointer to the ResourceDeserializer instance.
         */
        static ResourceDeserializer *GetInstance();

        /**
         * @brief Reset the singleton instance of the ResourceDeserializer.
         */
        static void ResetInstance();

        /**
         * @brief Set the path for resource serialization.
         * @param binaryFilePath The file path where resources are serialized.
         */
        void SetSerializationPath(const std::string &binaryFilePath);

        /**
         * @brief Get the path for resource serialization.
         * @return The file path where resources are serialized.
         */
        std::string GetSerializationPath() const;

        /**
         * @brief Register a resource type with a key and constructor.
         * @tparam T The resource type to register.
         * @param key The key associated with the resource type.
         * @param constructor The constructor function for creating instances of the resource type.
         */
        template <typename T>
        void RegisterResource(const std::string &key, std::function<std::unique_ptr<ISerializableResource>(void)> constructor);

        /**
         * @brief Unregister a resource type by its key.
         * @param key The key of the resource type to unregister.
         */
        void UnregisterResource(const std::string &key);

        /**
         * @brief Unregister all resource types.
         */
        void UnregisterAll();

        /**
         * @brief Check if a resource type is registered with a given key.
         * @param key The key to check for.
         * @return True if the key is registered, false otherwise.
         */
        bool HasSerializationKey(const std::string &key) const;

        /**
         * @brief Deserialize a resource by its key.
         * @param key The key of the resource to deserialize.
         * @return A unique pointer to the deserialized resource.
         */
        std::unique_ptr<ISerializableResource> Deserialize(const std::string &key);

        /**
         * @brief Generate a resource instance by its key.
         * @param key The key of the resource to generate.
         * @return A unique pointer to the generated resource.
         */
        std::unique_ptr<ISerializableResource> GenerateResource(const std::string &key) const;

    private:
        /**
         * @brief Default constructor for the ResourceDeserializer class.
         *
         * Private to enforce the singleton pattern.
         */
        ResourceDeserializer();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        ResourceDeserializer(const ResourceDeserializer &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        ResourceDeserializer &operator=(const ResourceDeserializer &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        ResourceDeserializer(ResourceDeserializer &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        ResourceDeserializer &operator=(ResourceDeserializer &&) = delete;

        /**
         * @brief Get the file extension for resource files.
         * @return A string representing the resource file extension.
         */
        std::string GetResourceExtension() const;

        /** @brief Pointer to the singleton instance of the ResourceDeserializer. */
        static ResourceDeserializer *instance_;

        /** @brief Path for resource serialization. */
        Path serializationPath_;

        /** @brief Map of keys to resource constructor functions. */
        mutable std::map<std::string, std::function<std::unique_ptr<ISerializableResource>(void)>> keyToResourceMap_;
    };

#include "FilesystemAdapters/ResourceDeserializer.hpp"

} // end namespace filesystem_adapters

#endif // filesystem_adapters_resourcedeserializer_h