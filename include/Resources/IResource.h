/**
 * @file IResource.h
 * @brief Declaration of the IResource interface for managing array data and tracking modifications.
 */

#ifndef resource_iresource_h
#define resource_iresource_h

#include <vector>

#include "Resources/config.h"

namespace resource
{

    /**
     * @class IResource
     * @brief An interface for managing array data and tracking its modification state.
     */
    class RESOURCE_DLL_EXPORT IResource
    {
    public:
        /**
         * @brief Default constructor for the IResource class.
         */
        IResource();

        /**
         * @brief Destructor for the IResource class.
         */
        virtual ~IResource();

        /**
         * @brief Copy constructor.
         * @param other The IResource instance to copy from.
         */
        IResource(const IResource &other);

        /**
         * @brief Copy assignment operator.
         * @param other The IResource instance to copy from.
         * @return Reference to the updated IResource instance.
         */
        IResource &operator=(const IResource &other);

        /**
         * @brief Move constructor.
         * @param other The IResource instance to move from.
         */
        IResource(IResource &&other);

        /**
         * @brief Move assignment operator.
         * @param other The IResource instance to move from.
         * @return Reference to the updated IResource instance.
         */
        IResource &operator=(IResource &&other);

        /**
         * @brief Get the number of columns in the resource data.
         * @return The column size.
         */
        size_t GetColumnSize() const;

        /**
         * @brief Get the number of rows in the resource data.
         * @return The row size.
         */
        size_t GetRowSize() const;

        /**
         * @brief Check if the resource data has been modified.
         * @return True if the data is dirty, false otherwise.
         */
        bool GetDirty() const;

        /**
         * @brief Get the size of each element in the resource data.
         * @return The size of an individual element.
         */
        virtual size_t GetElementSize() const = 0;

        /**
         * @brief Access the resource data as a mutable pointer.
         * @return A pointer to the resource data.
         */
        virtual void *Data() = 0;

        /**
         * @brief Access the resource data as a constant pointer.
         * @return A constant pointer to the resource data.
         */
        virtual const void *Data() const = 0;

        /**
         * @brief Assign new data to the resource.
         * @param buff A pointer to the new data.
         * @param n The size of the new data in bytes.
         */
        virtual void Assign(const char *buff, const size_t n) = 0;

    protected:
        /**
         * @brief Set the number of columns in the resource data.
         * @param size The new column size.
         */
        void SetColumnSize(const size_t size);

        /**
         * @brief Set the number of rows in the resource data.
         * @param size The new row size.
         */
        void SetRowSize(const size_t size);

        /**
         * @brief Update the checksum of the resource data.
         * @return True if the checksum was updated successfully, false otherwise.
         */
        bool UpdateChecksum() const;

        /**
         * @brief Get the checksum of the resource data.
         * @return The checksum value.
         */
        int Checksum() const;

    private:
        /** @brief The number of columns in the resource data. */
        size_t M_{0};

        /** @brief The number of rows in the resource data. */
        size_t N_{0};

        /** @brief The checksum of the resource data. */
        mutable int checkSum_{-1};

        /** @brief Flag indicating whether the resource data has been modified. */
        mutable bool dirty_{true};
    };

} // end namespace resource

#endif // end resource_iresource_h