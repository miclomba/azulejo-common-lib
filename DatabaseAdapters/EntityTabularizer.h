/**
 * @file EntityTabularizer.h
 * @brief Declaration of the EntityTabularizer class for tabularizing entities into database tables.
 */

#ifndef database_adapters_entitytabularizer_h
#define database_adapters_entitytabularizer_h

#include <string>
#include "Config/filesystem.h"

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "ITabularizableEntity.h"
#include "Sqlite.h"

namespace database_adapters {

/**
 * @class EntityTabularizer
 * @brief A singleton class responsible for tabularizing entities into SQLite database tables.
 */
class DATABASE_ADAPTERS_DLL_EXPORT EntityTabularizer
{
public:
    /**
     * @brief Destructor for the EntityTabularizer class.
     */
    virtual ~EntityTabularizer();

    /**
     * @brief Get the singleton instance of the EntityTabularizer.
     * @return Pointer to the EntityTabularizer instance.
     */
    static EntityTabularizer* GetInstance();

    /**
     * @brief Reset the singleton instance of the EntityTabularizer.
     */
    static void ResetInstance();

    /**
     * @brief Tabularize an entity into the database.
     * @param entity The tabularizable entity to be saved as a database table.
     */
    void Tabularize(const ITabularizableEntity& entity);

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

private:
    /**
     * @brief Default constructor for the EntityTabularizer class.
     *
     * Private to enforce the singleton pattern.
     */
    EntityTabularizer();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    EntityTabularizer(const EntityTabularizer&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     * @return Reference to the updated instance (not used).
     */
    EntityTabularizer& operator=(const EntityTabularizer&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving.
     */
    EntityTabularizer(EntityTabularizer&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving.
     * @return Reference to the updated instance (not used).
     */
    EntityTabularizer& operator=(EntityTabularizer&&) = delete;

    /**
     * @brief Tabularize an entity with an optional parent key.
     * @param entity The entity to tabularize.
     * @param parentKey The key of the parent entity (default is an empty string).
     */
    void TabularizeWithParentKey(const ITabularizableEntity& entity, const entity::Entity::Key& parentKey = "");

    /** @brief Pointer to the singleton instance of the EntityTabularizer. */
    static EntityTabularizer* instance_;

    /** @brief Hierarchy of tabularized entities. */
    entity::EntityHierarchy hierarchy_;

    /** @brief SQLite database adapter. */
    Sqlite databaseAdapter_;
};

} // end namespace database_adapters

#endif // database_adapters_entitytabularizer_h