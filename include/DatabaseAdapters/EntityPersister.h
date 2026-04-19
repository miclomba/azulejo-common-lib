/**
 * @file EntityPersister.h
 * @brief Declaration of the EntityPersister class for persisting entities into database tables.
 */

#ifndef database_adapters_entitypersister_h
#define database_adapters_entitypersister_h

#include <string>
#include <string_view>
#include "Config/filesystem.h"

#include "DatabaseAdapters/config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "DatabaseAdapters/IPersistableEntity.h"
#include "DatabaseAdapters/Sqlite.h"

namespace database_adapters
{

    /**
     * @class EntityPersister
     * @brief A singleton class responsible for persisting entities into SQLite database tables.
     */
    class DATABASE_ADAPTERS_DLL_EXPORT EntityPersister
    {
    public:
        /**
         * @brief Destructor for the EntityPersister class.
         */
        virtual ~EntityPersister() noexcept;

        /**
         * @brief Get the singleton instance of the EntityPersister.
         * @return Pointer to the EntityPersister instance.
         */
        static EntityPersister *GetInstance();

        /**
         * @brief Reset the singleton instance of the EntityPersister.
         */
        static void ResetInstance();

        /**
         * @brief Persist an entity into the database.
         * @param entity The persistable entity to be saved as a database table.
         */
        void Persist(const IPersistableEntity &entity);

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

    private:
        /**
         * @brief Default constructor for the EntityPersister class.
         *
         * Private to enforce the singleton pattern.
         */
        EntityPersister();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        EntityPersister(const EntityPersister &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        EntityPersister &operator=(const EntityPersister &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        EntityPersister(EntityPersister &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        EntityPersister &operator=(EntityPersister &&) = delete;

        /**
         * @brief Persist an entity with an optional parent key.
         * @param entity The entity to persist.
         * @param parentKey The key of the parent entity (default is an empty string).
         */
        void PersistWithParentKey(const IPersistableEntity &entity, const std::string_view parentKey = "");

        /** @brief Pointer to the singleton instance of the EntityPersister. */
        static EntityPersister *instance_;

        /** @brief Hierarchy of persisted entities. */
        entity::EntityHierarchy hierarchy_;

        /** @brief SQLite database adapter. */
        Sqlite databaseAdapter_;
    };

} // end namespace database_adapters

#endif // database_adapters_entitypersister_h