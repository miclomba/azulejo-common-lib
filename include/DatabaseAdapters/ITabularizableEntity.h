/**
 * @file ITabularizableEntity.h
 * @brief Declaration of the ITabularizableEntity interface for database tabularization.
 */

#ifndef database_adapters_itabularizableentity_h
#define database_adapters_itabularizableentity_h

#include <map>
#include <memory>
#include <string>

#include "DatabaseAdapters/config.h"
#include <boost/property_tree/ptree.hpp>
#include "Entities/Entity.h"

namespace database_adapters
{

    class EntityDetabularizer;
    class EntityTabularizer;
    class Sqlite;

    /**
     * @class ITabularizableEntity
     * @brief An interface for entities that can be saved to and loaded from database tables.
     *
     * Inherits from the `entity::Entity` class and provides methods for tabular data management.
     */
    class DATABASE_ADAPTERS_DLL_EXPORT ITabularizableEntity : public virtual entity::Entity
    {
    public:
        /**
         * @brief Allows EntityTabularizer and EntityDetabularizer access to private and protected members.
         */
        friend class EntityTabularizer;
        friend class EntityDetabularizer;

    public:
        /**
         * @brief Default constructor for the ITabularizableEntity class.
         */
        ITabularizableEntity();

        /**
         * @brief Destructor for the ITabularizableEntity class.
         */
        virtual ~ITabularizableEntity();

        /**
         * @brief Copy constructor.
         * @param other The ITabularizableEntity instance to copy from.
         */
        ITabularizableEntity(const ITabularizableEntity &other);

        /**
         * @brief Copy assignment operator.
         * @param other The ITabularizableEntity instance to copy from.
         * @return Reference to the updated ITabularizableEntity instance.
         */
        ITabularizableEntity &operator=(const ITabularizableEntity &other);

        /**
         * @brief Move constructor.
         * @param other The ITabularizableEntity instance to move from.
         */
        ITabularizableEntity(ITabularizableEntity &&other);

        /**
         * @brief Move assignment operator.
         * @param other The ITabularizableEntity instance to move from.
         * @return Reference to the updated ITabularizableEntity instance.
         */
        ITabularizableEntity &operator=(ITabularizableEntity &&other);

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

#endif // database_adapters_itabularizableentity_h