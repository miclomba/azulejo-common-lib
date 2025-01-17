/**
 * @file EntityDetabularizer.h
 * @brief Declaration of the EntityDetabularizer class for detabularizing entities from database tables.
 */

#ifndef database_adapters_entitydetabularizer_h
#define database_adapters_entitydetabularizer_h

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "Config/filesystem.h"

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "Entities/EntityRegistry.h"
#include "ITabularizableEntity.h"
#include "Sqlite.h"

namespace database_adapters {

/**
 * @class EntityDetabularizer
 * @brief A singleton class responsible for detabularizing entities from SQLite database tables.
 */
class DATABASE_ADAPTERS_DLL_EXPORT EntityDetabularizer
{
public:
    /**
     * @brief Destructor for the EntityDetabularizer class.
     */
    virtual ~EntityDetabularizer();

    /**
     * @brief Get the singleton instance of the EntityDetabularizer.
     * @return Pointer to the EntityDetabularizer instance.
     */
    static EntityDetabularizer* GetInstance();

    /**
     * @brief Reset the singleton instance of the EntityDetabularizer.
     */
    static void ResetInstance();

    /**
     * @brief Get the registry of tabularizable entities.
     * @return Reference to the EntityRegistry for ITabularizableEntity.
     */
    entity::EntityRegistry<ITabularizableEntity>& GetRegistry();

    /**
     * @brief Get the hierarchy of tabularized entities.
     * @return Reference to the EntityHierarchy.
     */
    entity::EntityHierarchy& GetHierarchy();

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
     * @brief Load and detabularize an entity from the database.
     * @param entity The tabularizable entity to load data into.
     */
    void LoadEntity(ITabularizableEntity& entity);

private:
    /**
     * @brief Default constructor for the EntityDetabularizer class.
     *
     * Private to enforce the singleton pattern.
     */
    EntityDetabularizer();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    EntityDetabularizer(const EntityDetabularizer&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     * @return Reference to the updated instance (not used).
     */
    EntityDetabularizer& operator=(const EntityDetabularizer&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving.
     */
    EntityDetabularizer(EntityDetabularizer&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving.
     * @return Reference to the updated instance (not used).
     */
    EntityDetabularizer& operator=(EntityDetabularizer&&) = delete;

    /**
     * @brief Load an entity with an optional parent key.
     * @param entity The entity to load data into.
     * @param parentKey The key of the parent entity (default is an empty string).
     */
    void LoadWithParentKey(ITabularizableEntity& entity, const entity::Entity::Key& parentKey = "");

    /** @brief Pointer to the singleton instance of the EntityDetabularizer. */
    static EntityDetabularizer* instance_;

    /** @brief Registry of tabularizable entities. */
    entity::EntityRegistry<ITabularizableEntity> registry_;

    /** @brief Hierarchy of tabularized entities. */
    entity::EntityHierarchy hierarchy_;

    /** @brief SQLite database adapter. */
    Sqlite databaseAdapter_;
};

} // end namespace database_adapters

#endif // database_adapters_entitydetabularizer_h