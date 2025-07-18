/**
 * @file ISerializableResource.h
 * @brief Declaration of the ISerializableResource interface for serializable resources.
 */

#ifndef filesystem_adapters_iserializableresource_h
#define filesystem_adapters_iserializableresource_h

#include <mutex>

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
         * @class LockedResource
         * @brief A synchronization proxy using the monitor idiom.
         */
        class LockedResource
        {
        public:
            /**
             * @brief Allows ResourceSerializer and ResourceDeserializer access to private and protected members.
             */
            friend class ResourceSerializer;
            friend class ResourceDeserializer;

            LockedResource(ISerializableResource &o);

            size_t GetColumnSize() const;
            size_t GetRowSize() const;
            bool GetDirty() const;
            size_t GetElementSize() const;
            void *Data();
            const void *Data() const;
            void Assign(const char *buff, const size_t n);

        protected:
            void SetColumnSize(const size_t size);
            void SetRowSize(const size_t size);
            bool UpdateChecksum() const;
            int Checksum() const;

        private:
            ISerializableResource *obj_;
            std::unique_lock<std::mutex> lk_;
        };

        /**
         * @brief RAII lock for this resource.
         * @return a lock.
         */
        LockedResource Lock();

        /**
         * @brief RAII lock for this resource.
         * @return a lock.
         */
        const LockedResource Lock() const;

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
        ISerializableResource(const ISerializableResource &other) = delete;

        /**
         * @brief Copy assignment operator.
         * @param other The ISerializableResource instance to copy from.
         * @return Reference to the updated ISerializableResource instance.
         */
        ISerializableResource &operator=(const ISerializableResource &other) = delete;

        /**
         * @brief Move constructor.
         * @param other The ISerializableResource instance to move from.
         */
        ISerializableResource(ISerializableResource &&other) = delete;

        /**
         * @brief Move assignment operator.
         * @param other The ISerializableResource instance to move from.
         * @return Reference to the updated ISerializableResource instance.
         */
        ISerializableResource &operator=(ISerializableResource &&other) = delete;

        /** @brief IO lock */
        std::mutex mtx_;
    };

} // end namespace filesystem_adapters

#endif // end filesystem_adapters_iserializableresource_h