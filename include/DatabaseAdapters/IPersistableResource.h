/**
 * @file IPersistableResource.h
 * @brief Declaration of the IPersistableResource interface for database persistence of resources.
 */

#ifndef database_adapters_ipersistableresource_h
#define database_adapters_ipersistableresource_h

#include "DatabaseAdapters/config.h"
#include "Resources/IResource.h"

namespace database_adapters
{

    class ResourceLoader;
    class ResourcePersister;

    /**
     * @class IPersistableResource
     * @brief An interface for resources that can be represented in database tables.
     *
     * Inherits from the `resource::IResource` class and provides a foundation for tabular database operations.
     */
    class DATABASE_ADAPTERS_DLL_EXPORT IPersistableResource : public virtual resource::IResource
    {
    public:
        /**
         * @brief Allows ResourcePersister and ResourceLoader access to private and protected members.
         */
        friend class ResourcePersister;
        friend class ResourceLoader;

    public:
        /**
         * @brief Default constructor for the IPersistableResource class.
         */
        IPersistableResource();

        /**
         * @brief Destructor for the IPersistableResource class.
         */
        virtual ~IPersistableResource();

        /**
         * @brief Copy constructor.
         * @param other The IPersistableResource instance to copy from.
         */
        IPersistableResource(const IPersistableResource &other);

        /**
         * @brief Copy assignment operator.
         * @param other The IPersistableResource instance to copy from.
         * @return Reference to the updated IPersistableResource instance.
         */
        IPersistableResource &operator=(const IPersistableResource &other);

        /**
         * @brief Move constructor.
         * @param other The IPersistableResource instance to move from.
         */
        IPersistableResource(IPersistableResource &&other);

        /**
         * @brief Move assignment operator.
         * @param other The IPersistableResource instance to move from.
         * @return Reference to the updated IPersistableResource instance.
         */
        IPersistableResource &operator=(IPersistableResource &&other);
    };

} // end namespace database_adapters

#endif // database_adapters_ipersistableresource_h
