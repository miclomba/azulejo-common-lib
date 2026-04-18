/**
 * @file EntityLoader.h
 * @brief Declaration of the EntityLoader class for loading entities from database tables.
 */

#ifndef database_adapters_entityloader_h
#define database_adapters_entityloader_h

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include "Config/filesystem.h"

#include "DatabaseAdapters/config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "Entities/EntityRegistry.h"
#include "DatabaseAdapters/IPersistableEntity.h"
#include "DatabaseAdapters/Sqlite.h"

namespace database_adapters
{

    /**
     * @class EntityLoader
     * @brief A singleton class responsible for loading entities from SQLite database tables.
     */
    class DATABASE_ADAPTERS_DLL_EXPORT EntityLoader
    {
    public:
        /**
         * @brief Destructor for the EntityLoader class.
         */
        virtual ~EntityLoader();

        /**
         * @brief Get the singleton instance of the EntityLoader.
         * @return Pointer to the EntityLoader instance.
         */
        static EntityLoader *GetInstance();

        /**
         * @brief Reset the singleton instance of the EntityLoader.
         */
        static void ResetInstance();

        /**
         * @brief Get the registry of persistable entities.
         * @return Reference to the EntityRegistry for IPersistableEntity.
         */
        entity::EntityRegistry<IPersistableEntity> &GetRegistry();

        /**
         * @brief Get the hierarchy of persisted entities.
         * @return Reference to the EntityHierarchy.
         */
        entity::EntityHierarchy &GetHierarchy();

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
         * @brief Load an entity from the database.
         * @param entity The persistable entity to load data into.
         */
        void LoadEntity(IPersistableEntity &entity);

    private:
        /**
         * @brief Default constructor for the EntityLoader class.
         *
         * Private to enforce the singleton pattern.
         */
        EntityLoader();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        EntityLoader(const EntityLoader &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        EntityLoader &operator=(const EntityLoader &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        EntityLoader(EntityLoader &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        EntityLoader &operator=(EntityLoader &&) = delete;

        /**
         * @brief Load an entity with an optional parent key.
         * @param entity The entity to load data into.
         * @param parentKey The key of the parent entity (default is an empty string).
         */
        void LoadWithParentKey(IPersistableEntity &entity, const std::string_view parentKey = "");

        /** @brief Pointer to the singleton instance of the EntityLoader. */
        static EntityLoader *instance_;

        /** @brief Registry of persistable entities. */
        entity::EntityRegistry<IPersistableEntity> registry_;

        /** @brief Hierarchy of persisted entities. */
        entity::EntityHierarchy hierarchy_;

        /** @brief SQLite database adapter. */
        Sqlite databaseAdapter_;
    };

} // end namespace database_adapters

#endif // database_adapters_entityloader_h