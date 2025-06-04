/**
 * @file ITabularizableResource.h
 * @brief Declaration of the ITabularizableResource interface for database tabularization of resources.
 */

#ifndef database_adapters_itabularizableresource_h
#define database_adapters_itabularizableresource_h

#include "DatabaseAdapters/config.h"
#include "Resources/IResource.h"

namespace database_adapters
{

    class ResourceDetabularizer;
    class ResourceTabularizer;

    /**
     * @class ITabularizableResource
     * @brief An interface for resources that can be represented in database tables.
     *
     * Inherits from the `resource::IResource` class and provides a foundation for tabular database operations.
     */
    class DATABASE_ADAPTERS_DLL_EXPORT ITabularizableResource : public virtual resource::IResource
    {
    public:
        /**
         * @brief Allows ResourceTabularizer and ResourceDetabularizer access to private and protected members.
         */
        friend class ResourceTabularizer;
        friend class ResourceDetabularizer;

    public:
        /**
         * @brief Default constructor for the ITabularizableResource class.
         */
        ITabularizableResource();

        /**
         * @brief Destructor for the ITabularizableResource class.
         */
        virtual ~ITabularizableResource();

        /**
         * @brief Copy constructor.
         * @param other The ITabularizableResource instance to copy from.
         */
        ITabularizableResource(const ITabularizableResource &other);

        /**
         * @brief Copy assignment operator.
         * @param other The ITabularizableResource instance to copy from.
         * @return Reference to the updated ITabularizableResource instance.
         */
        ITabularizableResource &operator=(const ITabularizableResource &other);

        /**
         * @brief Move constructor.
         * @param other The ITabularizableResource instance to move from.
         */
        ITabularizableResource(ITabularizableResource &&other);

        /**
         * @brief Move assignment operator.
         * @param other The ITabularizableResource instance to move from.
         * @return Reference to the updated ITabularizableResource instance.
         */
        ITabularizableResource &operator=(ITabularizableResource &&other);
    };

} // end namespace database_adapters

#endif // database_adapters_itabularizableresource_h
