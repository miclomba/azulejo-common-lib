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

class INTRAPROCESS_DLL_EXPORT ThreadPool
{
public:
	ThreadPool(const size_t numThreads);

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;

	virtual ~ThreadPool();

	size_t GetThreadCount() const;
	size_t GetTaskCount();

	template<typename T, typename... Args>
	void Post(std::function<T(Args...)> task, Args... args);

	void Stop();
	void Join();

private:

	static void RunTask(ThreadPool* pool);

	bool KeepRunning() const;
	bool Die() const;
	bool HasWorkThreadSafe();
	bool HasWorkThreadUnsafe() const;
	bool ThreadsShouldProceed();
	void AddToThreadCounter(const size_t val);

	std::atomic<size_t> numThreads_{ 0 };
	std::atomic<bool> keepRunning_{ true };
	std::atomic<bool> die_{ false };
	std::mutex lock_;
	std::condition_variable threadNotifier_;
	std::queue<std::packaged_task<void()>> workQueue_;
	std::vector<std::thread> workerThreads_;
};

#include "ThreadPool.hpp"

} // end intraprocess
#endif // intraprocess_thread_pool_h
