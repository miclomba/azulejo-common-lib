/**
 * @file ResourceDetabularizer.h
 * @brief Declaration of the ResourceDetabularizer class for detabularizing resources from database tables.
 */

#ifndef database_adapters_resourcedetabularizer_h
#define database_adapters_resourcedetabularizer_h

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include "Config/filesystem.h"

#include "config.h"

#include "ITabularizableResource.h"
#include "Sqlite.h"

namespace database_adapters {

/**
 * @class ResourceDetabularizer
 * @brief A singleton class responsible for detabularizing resources from SQLite database tables.
 */
class DATABASE_ADAPTERS_DLL_EXPORT ResourceDetabularizer
{
public:
    /**
     * @brief Destructor for the ResourceDetabularizer class.
     */
    virtual ~ResourceDetabularizer();

    /**
     * @brief Get the singleton instance of the ResourceDetabularizer.
     * @return Pointer to the ResourceDetabularizer instance.
     */
    static ResourceDetabularizer* GetInstance();

    /**
     * @brief Reset the singleton instance of the ResourceDetabularizer.
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
    void OpenDatabase(const Path& dbPath);

    /**
     * @brief Get the database adapter.
     * @return Reference to the SQLite database adapter.
     */
    Sqlite& GetDatabase();

    /**
     * @brief Register a resource type with a key and constructor.
     * @tparam T The resource type to register.
     * @param key The key associated with the resource type.
     * @param constructor The constructor function for creating instances of the resource type.
     */
    template<typename T>
    void RegisterResource(const std::string& key, std::function<std::unique_ptr<ITabularizableResource>(void)> constructor);

    /**
     * @brief Unregister a resource type by its key.
     * @param key The key of the resource type to unregister.
     */
    void UnregisterResource(const std::string& key);

    /**
     * @brief Unregister all resource types.
     */
    void UnregisterAll();

    /**
     * @brief Check if a resource type is registered with a given key.
     * @param key The key to check for.
     * @return True if the key is registered, false otherwise.
     */
    bool HasTabularizationKey(const std::string& key) const;

    /**
     * @brief Detabularize a resource by its key.
     * @param key The key of the resource to detabularize.
     * @return A unique pointer to the detabularized resource.
     */
    std::unique_ptr<ITabularizableResource> Detabularize(const std::string& key);

    /**
     * @brief Generate a resource instance by its key.
     * @param key The key of the resource to generate.
     * @return A unique pointer to the generated resource.
     */
    std::unique_ptr<ITabularizableResource> GenerateResource(const std::string& key) const;

private:
    /**
     * @brief Default constructor for the ResourceDetabularizer class.
     *
     * Private to enforce the singleton pattern.
     */
    ResourceDetabularizer();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    ResourceDetabularizer(const ResourceDetabularizer&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     * @return Reference to the updated instance (not used).
     */
    ResourceDetabularizer& operator=(const ResourceDetabularizer&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving.
     */
    ResourceDetabularizer(ResourceDetabularizer&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving.
     * @return Reference to the updated instance (not used).
     */
    ResourceDetabularizer& operator=(ResourceDetabularizer&&) = delete;

    /** @brief Pointer to the singleton instance of the ResourceDetabularizer. */
    static ResourceDetabularizer* instance_;

    /** @brief Map of keys to resource constructor functions. */
    mutable std::map<std::string, std::function<std::unique_ptr<ITabularizableResource>(void)>> keyToResourceMap_;

    /** @brief SQLite database adapter. */
    Sqlite databaseAdapter_;
};

#include "ResourceDetabularizer.hpp"

} // end namespace database_adapters

#endif // database_adapters_resourcedetabularizer_h
