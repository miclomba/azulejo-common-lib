/**
 * @file IOMPRunnable.h
 * @brief Declaration of the IOMPRunnable class for running OpenMP-based parallel tasks.
 */

#ifndef intraprocess_iomp_runnable_h
#define intraprocess_iomp_runnable_h

#include <cstddef>

#include "config.h"

namespace intraprocess {

/**
 * @class IOMPRunnable
 * @brief A base class for running OpenMP-based parallel tasks with customizable thread settings.
 */
class INTRAPROCESS_DLL_EXPORT IOMPRunnable
{
public:
    /**
     * @brief Constructor for the IOMPRunnable class.
     * @param numThreads The number of threads to use for the OpenMP task.
     * @param setDynamic Whether to enable dynamic adjustment of threads (default: true).
     * @param setNested Whether to enable nested parallelism (default: true).
     */
    IOMPRunnable(const size_t numThreads, const bool setDynamic = true, const bool setNested = true);

    /**
     * @brief Virtual destructor for the IOMPRunnable class.
     */
    virtual ~IOMPRunnable();

    /**
     * @brief Copy constructor.
     * @param other The IOMPRunnable instance to copy from.
     */
    IOMPRunnable(const IOMPRunnable& other);

    /**
     * @brief Copy assignment operator.
     * @param other The IOMPRunnable instance to copy from.
     * @return Reference to the updated IOMPRunnable instance.
     */
    IOMPRunnable& operator=(const IOMPRunnable& other);

    /**
     * @brief Move constructor.
     * @param other The IOMPRunnable instance to move from.
     */
    IOMPRunnable(IOMPRunnable&& other);

    /**
     * @brief Move assignment operator.
     * @param other The IOMPRunnable instance to move from.
     * @return Reference to the updated IOMPRunnable instance.
     */
    IOMPRunnable& operator=(IOMPRunnable&& other);

    /**
     * @brief Start the OpenMP task by invoking the Run method.
     */
    void Start();

    /**
     * @brief Get the number of threads configured for the OpenMP task.
     * @return The number of threads.
     */
    size_t GetNumThreads() const;

    /**
     * @brief Get the dynamic state of the OpenMP task.
     * @return True if dynamic thread adjustment is enabled, false otherwise.
     */
    bool GetDynamicState() const;

    /**
     * @brief Get the nested state of the OpenMP task.
     * @return True if nested parallelism is enabled, false otherwise.
     */
    bool GetNestedState() const;

protected:
    /**
     * @brief The method containing the OpenMP task logic.
     *
     * This method must be overridden by derived classes to define the specific task to execute.
     */
    virtual void Run() = 0;

    /** @brief The number of threads to use for the OpenMP task. */
    size_t numThreads_;

    /** @brief Flag indicating whether dynamic thread adjustment is enabled. */
    bool setDynamic_; 

    /** @brief Flag indicating whether nested parallelism is enabled. */
    bool setNested_;
};

} // namespace intraprocess

#endif // intraprocess_iomp_runnable_h
