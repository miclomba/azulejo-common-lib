/**
 * @file ISharedMemory.h
 * @brief Declaration of the ISharedMemory interface for managing shared memory segments.
 */

#ifndef interprocess_isharedmemory_h
#define interprocess_isharedmemory_h

#include <memory>
#include <string>

#include "Interprocess/config.h"

#include <boost/interprocess/shared_memory_object.hpp>

namespace interprocess
{

    /**
     * @class ISharedMemory
     * @brief An interface for managing shared memory segments in an interprocess communication context.
     *
     * Provides methods for creating, opening, and destroying shared memory, as well as accessing its properties.
     */
    class INTERPROCESS_DLL_EXPORT ISharedMemory
    {
    public:
        /**
         * @brief Default constructor for the ISharedMemory class.
         */
        ISharedMemory();

        /**
         * @brief Virtual destructor for the ISharedMemory class.
         */
        virtual ~ISharedMemory();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        ISharedMemory(const ISharedMemory &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         * @return Reference to the updated instance (not used).
         */
        ISharedMemory &operator=(const ISharedMemory &) = delete;

        /**
         * @brief Deleted move constructor to prevent moving.
         */
        ISharedMemory(ISharedMemory &&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving.
         * @return Reference to the updated instance (not used).
         */
        ISharedMemory &operator=(ISharedMemory &&) = delete;

        /**
         * @brief Get the name of the shared memory segment.
         * @return A string representing the name of the shared memory segment.
         */
        std::string GetSharedName() const;

        /**
         * @brief Get the size of the shared memory segment.
         * @return The size of the shared memory segment in bytes.
         */
        size_t GetSharedSize() const;

        /**
         * @brief Get the address of the shared memory segment.
         * @return A pointer to the address of the shared memory segment.
         */
        void *GetSharedAddress() const;

        /**
         * @brief Check if the current instance is the owner of the shared memory segment.
         * @return True if the current instance owns the shared memory segment, false otherwise.
         */
        bool IsSharedMemoryOwner() const;

        /**
         * @brief Destroy the shared memory segment.
         * @return True if the shared memory segment was successfully destroyed, false otherwise.
         */
        bool Destroy();

        /**
         * @brief Create a shared memory segment.
         * @param name The name of the shared memory segment.
         * @param size The size of the shared memory segment in bytes.
         */
        void Create(const std::string &name, const size_t size);

        /**
         * @brief Open an existing shared memory segment.
         * @param name The name of the shared memory segment to open.
         */
        void Open(const std::string &name);

    private:
        /** @brief Shared pointer to the Boost shared memory object. */
        std::shared_ptr<boost::interprocess::shared_memory_object> shmem_;

        /** @brief Flag indicating whether the current instance owns the shared memory segment. */
        bool isShmemOwner_{false};
    };

} // end namespace interprocess

#endif // interprocess_isharedmemory_h