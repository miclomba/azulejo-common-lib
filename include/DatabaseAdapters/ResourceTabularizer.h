/**
 * @file ResourceTabularizer.h
 * @brief Declaration of the ResourceTabularizer class for tabularizing resources into database tables.
 */

#ifndef database_adapters_resourcetabularizer_h
#define database_adapters_resourcetabularizer_h

#include <string>
#include "Config/filesystem.h"

#include "DatabaseAdapters/config.h"

#include "DatabaseAdapters/ITabularizableResource.h"
#include "DatabaseAdapters/Sqlite.h"

namespace database_adapters
{

    /**
     * @class ResourceTabularizer
     * @brief A singleton class responsible for tabularizing resources into SQLite database tables.
     */
    class DATABASE_ADAPTERS_DLL_EXPORT ResourceTabularizer
    {
    public:
        /**
         * @brief Destructor for the ResourceTabularizer class.
         */
        virtual ~ResourceTabularizer();

        /**
         * @brief Get the singleton instance of the ResourceTabularizer.
         * @return Pointer to the ResourceTabularizer instance.
         */
        static ResourceTabularizer *GetInstance();

        /**
         * @brief Reset the singleton instance of the ResourceTabularizer.
         */
        static void ResetInstance();

        /**
         * @brief Tabularize a resource into the database.
         * @param resource The tabularizable resource to save as a database table.
         * @param key The key associated with the resource.
         */
        void Tabularize(const ITabularizableResource &resource, const std::string &key);

        /**
         * @brief Remove a resource from the database by its key.
         * @param key The key associated with the resource to remove.
         */
        void Untabularize(const std::string &key);

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
         * @brief Default constructor for the ResourceTabularizer class.
         *
         * Private to enforce the singleton pattern.
         */
        ResourceTabularizer();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        ResourceTabularizer(const ResourceTabularizer &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        ResourceTabularizer &operator=(const ResourceTabularizer &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        ResourceTabularizer(ResourceTabularizer &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        ResourceTabularizer &operator=(ResourceTabularizer &&) = delete;

        /** @brief Pointer to the singleton instance of the ResourceTabularizer. */
        static ResourceTabularizer *instance_;

        /** @brief SQLite database adapter. */
        Sqlite databaseAdapter_;
    };

} // end namespace database_adapters

#endif // database_adapters_resourcetabularizer_h