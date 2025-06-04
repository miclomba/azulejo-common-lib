/**
 * @file ISerializableResource.h
 * @brief Declaration of the ISerializableResource interface for serializable resources.
 */

#ifndef filesystem_adapters_iserializableresource_h
#define filesystem_adapters_iserializableresource_h

#include "FilesystemAdapters/config.h"
#include "Resources/IResource.h"

namespace filesystem_adapters
{

    /**
     * @class ISerializableResource
     * @brief An interface for resources that can be serialized and deserialized.
     *
     * Inherits from the `resource::IResource` class and provides a foundation for serializable resource implementations.
     */
    class FILESYSTEM_ADAPTERS_DLL_EXPORT ISerializableResource : public virtual resource::IResource
    {
    public:
        /**
         * @brief Allows ResourceSerializer and ResourceDeserializer access to private and protected members.
         */
        friend class ResourceSerializer;
        friend class ResourceDeserializer;

        /**
         * @brief Default constructor for the ISerializableResource class.
         */
        ISerializableResource();

        /**
         * @brief Destructor for the ISerializableResource class.
         */
        virtual ~ISerializableResource();

        /**
         * @brief Copy constructor.
         * @param other The ISerializableResource instance to copy from.
         */
        ISerializableResource(const ISerializableResource &other);

        /**
         * @brief Copy assignment operator.
         * @param other The ISerializableResource instance to copy from.
         * @return Reference to the updated ISerializableResource instance.
         */
        ISerializableResource &operator=(const ISerializableResource &other);

        /**
         * @brief Move constructor.
         * @param other The ISerializableResource instance to move from.
         */
        ISerializableResource(ISerializableResource &&other);

        /**
         * @brief Move assignment operator.
         * @param other The ISerializableResource instance to move from.
         * @return Reference to the updated ISerializableResource instance.
         */
        ISerializableResource &operator=(ISerializableResource &&other);
    };

} // end namespace filesystem_adapters

#endif // end filesystem_adapters_iserializableresource_h