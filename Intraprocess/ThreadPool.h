/**
 * @file ThreadPool.h
 * @brief Declaration of the ThreadPool class for managing a pool of worker threads.
 */

#ifndef intraprocess_thread_pool_h
#define intraprocess_thread_pool_h

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include "config.h"

namespace intraprocess {

/**
 * @class ThreadPool
 * @brief A class for managing a pool of worker threads to execute tasks concurrently.
 */
class INTRAPROCESS_DLL_EXPORT ThreadPool
{
public:
    /**
     * @brief Constructor for the ThreadPool class.
     * @param numThreads The number of threads to initialize in the thread pool (default is 1).
     */
    ThreadPool(const size_t numThreads = 1);

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    ThreadPool(const ThreadPool&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     * @return Reference to the updated instance (not used).
     */
    ThreadPool& operator=(const ThreadPool&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving.
     */
    ThreadPool(ThreadPool&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving.
     * @return Reference to the updated instance (not used).
     */
    ThreadPool& operator=(ThreadPool&&) = delete;

    /**
     * @brief Destructor for the ThreadPool class.
     */
    virtual ~ThreadPool();

    /**
     * @brief Get the number of threads in the thread pool.
     * @return The number of threads.
     */
    size_t GetThreadCount() const;

    /**
     * @brief Get the number of tasks currently in the queue.
     * @return The number of tasks in the queue.
     */
    size_t GetTaskCount();

    /**
     * @brief Post a task to the thread pool for execution.
     * @tparam T The return type of the task function.
     * @tparam Params The parameter types of the task function.
     * @tparam Args The argument types to pass to the task function.
     * @param task The task function to execute.
     * @param args The arguments to pass to the task function.
     */
    template<typename T, typename... Params, typename... Args>
    void Post(std::function<T(Params&...)> task, Args&&... args);

    /**
     * @brief Stop the thread pool and prevent any further tasks from being posted.
     */
    void Stop();

    /**
     * @brief Join all threads in the thread pool, blocking until all tasks are completed.
     */
    void Join();

private:
    /**
     * @brief Execute tasks from the work queue.
     * @param pool Pointer to the ThreadPool instance.
     */
    static void RunTask(ThreadPool* pool);

    /**
     * @brief Check if threads should continue running.
     * @return True if threads should continue running, false otherwise.
     */
    bool KeepRunning() const;

    /**
     * @brief Check if the thread pool is shutting down.
     * @return True if the thread pool is shutting down, false otherwise.
     */
    bool Die() const;

    /**
     * @brief Check if there are tasks in the queue (thread-safe).
     * @return True if there are tasks in the queue, false otherwise.
     */
    bool HasWorkThreadSafe();

    /**
     * @brief Check if there are tasks in the queue (not thread-safe).
     * @return True if there are tasks in the queue, false otherwise.
     */
    bool HasWorkThreadUnsafe() const;

    /**
     * @brief Check if threads should proceed with their tasks.
     * @return True if threads should proceed, false otherwise.
     */
    bool ThreadsShouldProceed();

    /**
     * @brief Increment or decrement the thread counter.
     * @param val The value to add to the thread counter.
     */
    void AddToThreadCounter(const size_t val);

    /** @brief The number of threads in the thread pool. */
    std::atomic<size_t> numThreads_{ 0 };

    /** @brief Flag indicating whether the threads should keep running. */
    std::atomic<bool> keepRunning_{ true };

    /** @brief Flag indicating whether the thread pool is shutting down. */
    std::atomic<bool> die_{ false };

    /** @brief Mutex for synchronizing access to the work queue and thread state. */
    std::mutex lock_;

    /** @brief Condition variable for notifying threads about new tasks. */
    std::condition_variable threadNotifier_;

    /** @brief Queue of tasks to be executed by the threads. */
    std::queue<std::packaged_task<void()>> workQueue_;

    /** @brief Vector of worker threads. */
    std::vector<std::thread> workerThreads_;
};

#include "ThreadPool.hpp"

} // end namespace intraprocess

#endif // intraprocess_thread_pool_h