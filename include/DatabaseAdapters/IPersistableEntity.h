/**
 * @file IPersistableEntity.h
 * @brief Declaration of the IPersistableEntity interface for database persistence.
 */

#ifndef database_adapters_ipersistableentity_h
#define database_adapters_ipersistableentity_h

#include <map>
#include <memory>
#include <string>

#include "DatabaseAdapters/config.h"
#include <boost/property_tree/ptree.hpp>
#include "Entities/Entity.h"

namespace database_adapters
{

    class EntityLoader;
    class EntityPersister;
    class Sqlite;

    /**
     * @class IPersistableEntity
     * @brief An interface for entities that can be saved to and loaded from database tables.
     *
     * Inherits from the `entity::Entity` class and provides methods for tabular data management.
     */
    class DATABASE_ADAPTERS_DLL_EXPORT IPersistableEntity : public virtual entity::Entity
    {
    public:
        /**
         * @brief Allows EntityPersister and EntityLoader access to private and protected members.
         */
        friend class EntityPersister;
        friend class EntityLoader;

    public:
        /**
         * @brief Default constructor for the IPersistableEntity class.
         */
        IPersistableEntity();

        /**
         * @brief Destructor for the IPersistableEntity class.
         */
        virtual ~IPersistableEntity();

        /**
         * @brief Copy constructor.
         * @param other The IPersistableEntity instance to copy from.
         */
        IPersistableEntity(const IPersistableEntity &other);

        /**
         * @brief Copy assignment operator.
         * @param other The IPersistableEntity instance to copy from.
         * @return Reference to the updated IPersistableEntity instance.
         */
        IPersistableEntity &operator=(const IPersistableEntity &other);

        /**
         * @brief Move constructor.
         * @param other The IPersistableEntity instance to move from.
         */
        IPersistableEntity(IPersistableEntity &&other);

        /**
         * @brief Move assignment operator.
         * @param other The IPersistableEntity instance to move from.
         * @return Reference to the updated IPersistableEntity instance.
         */
        IPersistableEntity &operator=(IPersistableEntity &&other);

        /**
         * @brief Save the entity data to a database.
         * @param tree The property tree containing the entity's data.
         * @param database The SQLite database instance to save data into.
         */
        virtual void Save(boost::property_tree::ptree &tree, Sqlite &database) const = 0;

        /**
         * @brief Load the entity data from a database.
         * @param tree The property tree to populate with the entity's data.
         * @param database The SQLite database instance to load data from.
         */
        virtual void Load(boost::property_tree::ptree &tree, Sqlite &database) = 0;

    protected:
        /**
         * @brief Retrieve an aggregated member by its key.
         * @param key The key of the aggregated member to retrieve.
         * @return A shared pointer to the aggregated member.
         */
        SharedEntity &GetAggregatedMember(const Key &key) const override;
    };

} // end namespace database_adapters

#endif // database_adapters_ipersistableentity_h