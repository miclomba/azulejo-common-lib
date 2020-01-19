#ifndef intraprocess_thread_pool_h
#define intraprocess_thread_pool_h

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
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

	std::future<int> PostTask(std::packaged_task<int()>&& task);

	void Stop();
	void Join();

private:

	static void RunTask(ThreadPool* pool);

	bool StayAlive() const;
	bool HasWorkThreadSafe();
	bool HasWorkThreadUnsafe() const;
	bool ThreadsShouldProceed();
	void AddToThreadCounter(const size_t val);

	std::atomic<size_t> numThreads_{ 0 };
	std::atomic<bool> stayAlive_{ true };
	std::mutex lock_;
	std::condition_variable threadNotifier_;
	std::queue<std::packaged_task<int()>> workQueue_;
	std::vector<std::thread> workerThreads_;
};

} // end intraprocess
#endif // intraprocess_thread_pool_h
