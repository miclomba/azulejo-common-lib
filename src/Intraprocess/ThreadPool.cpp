#include "Intraprocess/ThreadPool.h"

#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

using intraprocess::ThreadPool;

namespace
{
	std::packaged_task<void()> PopWorkQueue(std::queue<std::packaged_task<void()>> &workQueue)
	{
		std::packaged_task<void()> task;
		task = std::move(workQueue.front());
		workQueue.pop();
		return task;
	}
} // end namespace

ThreadPool::ThreadPool(const size_t numThreads)
{
	if (numThreads == 0)
		throw std::runtime_error("Cannot construct ThreadPool with 0 threads");

	for (size_t i = 0; i < numThreads; ++i)
		workerThreads_.emplace_back(RunTask, this);

	while (GetThreadCount() != numThreads)
		continue;

	// allow time for all threads to enter waiting
	std::chrono::milliseconds HUNDRED_MSEC(100);
	std::this_thread::sleep_for(HUNDRED_MSEC);
}

ThreadPool::~ThreadPool()
{
	Join();
}

void ThreadPool::Stop()
{
	die_.store(true);

	for (std::thread &th : workerThreads_)
	{
		if (th.joinable())
			th.join();
	}
}

void ThreadPool::Join()
{
	{
		std::unique_lock<std::mutex> lock(lock_);
		keepRunning_.store(false);
		threadNotifier_.notify_all();
	}

	while (GetThreadCount() > 0)
		continue;

	for (std::thread &th : workerThreads_)
	{
		if (th.joinable())
			th.join();
	}
}

size_t ThreadPool::GetThreadCount() const
{
	return numThreads_.load();
}

size_t ThreadPool::GetTaskCount()
{
	std::unique_lock<std::mutex> lock(lock_);
	size_t count = workQueue_.size();
	threadNotifier_.notify_one();
	return count;
}

bool ThreadPool::KeepRunning() const
{
	return keepRunning_.load();
}

bool ThreadPool::Die() const
{
	return die_.load();
}

bool ThreadPool::HasWorkThreadSafe()
{
	std::unique_lock<std::mutex> lock(lock_);
	return HasWorkThreadUnsafe();
}

bool ThreadPool::HasWorkThreadUnsafe() const
{
	return !workQueue_.empty();
}

bool ThreadPool::ThreadsShouldProceed()
{
	return !KeepRunning() || HasWorkThreadUnsafe();
}

void ThreadPool::AddToThreadCounter(const size_t val)
{
	std::unique_lock<std::mutex> lock(lock_);
	numThreads_.store(numThreads_.load() + val);
}

void ThreadPool::RunTask(ThreadPool *pool)
{
	if (!pool)
		throw std::runtime_error("ThreadPool::Work was given a nullptr value");

	pool->AddToThreadCounter(1);

	while (!pool->Die() && (pool->KeepRunning() || pool->HasWorkThreadSafe()))
	{
		std::packaged_task<void()> task;
		{
			std::unique_lock<std::mutex> lock(pool->lock_);
			pool->threadNotifier_.wait(lock, [pool]()
									   { return pool->ThreadsShouldProceed(); });

			// check because the wake operation can wake up spuriously due to non-safe predicate
			if (pool->HasWorkThreadUnsafe())
				task = PopWorkQueue(pool->workQueue_);
		}

		if (task.valid())
			task();
	}

	pool->AddToThreadCounter(-1);
}
