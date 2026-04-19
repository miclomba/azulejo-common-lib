/**
 * @file ResourcePersister.h
 * @brief Declaration of the ResourcePersister class for persisting resources into database tables.
 */

#ifndef database_adapters_resourcepersister_h
#define database_adapters_resourcepersister_h

#include <string>
#include <string_view>
#include "Config/filesystem.h"

#include "DatabaseAdapters/config.h"

#include "DatabaseAdapters/IPersistableResource.h"
#include "DatabaseAdapters/Sqlite.h"

namespace database_adapters
{

    /**
     * @class ResourcePersister
     * @brief A singleton class responsible for persisting resources into SQLite database tables.
     */
    class DATABASE_ADAPTERS_DLL_EXPORT ResourcePersister
    {
    public:
        /**
         * @brief Destructor for the ResourcePersister class.
         */
        virtual ~ResourcePersister() noexcept;

        /**
         * @brief Get the singleton instance of the ResourcePersister.
         * @return Pointer to the ResourcePersister instance.
         */
        static ResourcePersister *GetInstance();

        /**
         * @brief Reset the singleton instance of the ResourcePersister.
         */
        static void ResetInstance();

        /**
         * @brief Persist a resource into the database.
         * @param resource The persistable resource to save as a database table.
         * @param key The key associated with the resource.
         */
        void Persist(const IPersistableResource &resource, const std::string_view key);

        /**
         * @brief Remove a resource from the database by its key.
         * @param key The key associated with the resource to remove.
         */
        void Load(const std::string_view key);

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

    private:
        /**
         * @brief Default constructor for the ResourcePersister class.
         *
         * Private to enforce the singleton pattern.
         */
        ResourcePersister();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        ResourcePersister(const ResourcePersister &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        ResourcePersister &operator=(const ResourcePersister &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        ResourcePersister(ResourcePersister &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        ResourcePersister &operator=(ResourcePersister &&) = delete;

        /** @brief Pointer to the singleton instance of the ResourcePersister. */
        static ResourcePersister *instance_;

        /** @brief SQLite database adapter. */
        Sqlite databaseAdapter_;
    };

} // end namespace database_adapters

#endif // database_adapters_resourcepersister_h