/**
 * @file ResourceLoader.h
 * @brief Declaration of the ResourceLoader class for loading resources from database tables.
 */

#ifndef database_adapters_resourceloader_h
#define database_adapters_resourceloader_h

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include "Config/filesystem.h"

#include "DatabaseAdapters/config.h"

#include "DatabaseAdapters/IPersistableResource.h"
#include "DatabaseAdapters/Sqlite.h"

namespace database_adapters
{

    /**
     * @class ResourceLoader
     * @brief A singleton class responsible for loading resources from SQLite database tables.
     */
    class DATABASE_ADAPTERS_DLL_EXPORT ResourceLoader
    {
    public:
        /**
         * @brief Destructor for the ResourceLoader class.
         */
        virtual ~ResourceLoader() noexcept;

        /**
         * @brief Get the singleton instance of the ResourceLoader.
         * @return Pointer to the ResourceLoader instance.
         */
        static ResourceLoader *GetInstance();

        /**
         * @brief Reset the singleton instance of the ResourceLoader.
         */
        static void ResetInstance();

        /**
         * @brief Close the currently opened database.
         */
        void CloseDatabase();

        /**
         * @brief Open a database file.
         * @param dbPath The path to the SQLite database file.
         */
        void OpenDatabase(const Path &dbPath);

        /**
         * @brief Get the database adapter.
         * @return Reference to the SQLite database adapter.
         */
        Sqlite &GetDatabase();

        /**
         * @brief Register a resource type with a key and constructor.
         * @tparam T The resource type to register.
         * @param key The key associated with the resource type.
         * @param constructor The constructor function for creating instances of the resource type.
         */
        template <typename T>
        void RegisterResource(const std::string_view key, std::function<std::unique_ptr<IPersistableResource>(void)> constructor);

        /**
         * @brief Unregister a resource type by its key.
         * @param key The key of the resource type to unregister.
         */
        void UnregisterResource(const std::string_view key);

        /**
         * @brief Unregister all resource types.
         */
        void UnregisterAll();

        /**
         * @brief Check if a resource type is registered with a given key.
         * @param key The key to check for.
         * @return True if the key is registered, false otherwise.
         */
        bool HasPersistenceKey(const std::string_view key) const;

        /**
         * @brief Load a resource by its key.
         * @param key The key of the resource to load.
         * @return A unique pointer to the loaded resource.
         */
        std::unique_ptr<IPersistableResource> Load(const std::string_view key);

        /**
         * @brief Generate a resource instance by its key.
         * @param key The key of the resource to generate.
         * @return A unique pointer to the generated resource.
         */
        std::unique_ptr<IPersistableResource> GenerateResource(const std::string_view key) const;

    private:
        /**
         * @brief Default constructor for the ResourceLoader class.
         *
         * Private to enforce the singleton pattern.
         */
        ResourceLoader();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        ResourceLoader(const ResourceLoader &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        ResourceLoader &operator=(const ResourceLoader &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        ResourceLoader(ResourceLoader &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        ResourceLoader &operator=(ResourceLoader &&) = delete;

        /** @brief Pointer to the singleton instance of the ResourceLoader. */
        static ResourceLoader *instance_;

        /** @brief Map of keys to resource constructor functions. */
        mutable std::map<std::string, std::function<std::unique_ptr<IPersistableResource>(void)>, std::less<>> keyToResourceMap_;

        /** @brief SQLite database adapter. */
        Sqlite databaseAdapter_;
    };

#include "DatabaseAdapters/ResourceLoader.hpp"

} // end namespace database_adapters

#endif // database_adapters_resourceloader_h
