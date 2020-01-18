#ifndef intraprocess_thread_pool_h
#define intraprocess_thread_pool_h

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

	std::future<int> PostTask(std::packaged_task<int()>&& task);

	void Shutdown();

private:

	static void RunTask(ThreadPool* pool);

	bool StayAlive() const;
	bool HasWork() const;
	bool ThreadsShouldProceed() const;

	size_t numThreads_{ 0 };
	bool stayAlive_{ true };
	std::mutex lock_;
	std::condition_variable threadNotifier_;
	std::queue<std::packaged_task<int()>> workQueue_;
	std::vector<std::thread> workerThreads_;
};

} // end intraprocess
#endif // intraprocess_thread_pool_h
