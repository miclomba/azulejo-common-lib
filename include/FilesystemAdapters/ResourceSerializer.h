/**
 * @file ResourceSerializer.h
 * @brief Declaration of the ResourceSerializer class for serializing and deserializing resources.
 */

#ifndef filesystem_adapters_resourceserializer_h
#define filesystem_adapters_resourceserializer_h

#include <fstream>
#include <mutex>
#include <stdexcept>
#include <string>
#include "Config/filesystem.h"

#include "FilesystemAdapters/config.h"
#include "FilesystemAdapters/ISerializableResource.h"

namespace filesystem_adapters
{

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
        static ResourceSerializer *GetInstance();

        /**
         * @brief Serialize a resource with the specified key.
         * @param resource The resource to serialize.
         * @param key The key associated with the resource.
         * @param serializationPath The file system path to the resource
         */
        void Serialize(const ISerializableResource &resource, const std::string &key, const std::string &serializationPath);

        /**
         * @brief Deserialize a resource with the specified key.
         * @param key The key associated with the resource to deserialize.
         * @param serializationPath The file system path to the resource
         */
        void Unserialize(const std::string &key, const std::string &serializationPath);

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
        ResourceSerializer(const ResourceSerializer &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        ResourceSerializer &operator=(const ResourceSerializer &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        ResourceSerializer(ResourceSerializer &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        ResourceSerializer &operator=(ResourceSerializer &&) = delete;

        /** @brief Mutex for IO. */
        std::mutex mtx_;
    };

} // end namespace filesystem_adapters

#endif // filesystem_adapters_resourceserializer_h
