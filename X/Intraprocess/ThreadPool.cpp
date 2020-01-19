#include "ThreadPool.h"

#include <functional>
#include <future>
#include <mutex>
#include <stdexcept>
#include <thread>

using intraprocess::ThreadPool;

namespace
{
std::packaged_task<int()> PopWorkQueue(std::queue<std::packaged_task<int()>>& workQueue)
{
	std::packaged_task<int()> task;
	task = std::move(workQueue.front());
	workQueue.pop();
	return task;
}
}

ThreadPool::ThreadPool(const size_t numThreads) :
	numThreads_(numThreads)
{
	if (numThreads == 0)
		throw std::runtime_error("Cannot construct ThreadPool with 0 threads");

	for (size_t i = 0; i < numThreads; ++i)
		workerThreads_.emplace_back(RunTask, this);
}

ThreadPool::~ThreadPool()
{
	Stop();
}

void ThreadPool::Stop()
{
	{
		std::unique_lock<std::mutex> lock(lock_);
		stayAlive_.store(false);
		threadNotifier_.notify_all();
	}
	Join();
}

void ThreadPool::Join()
{
	for (std::thread& th : workerThreads_)
	{
		if (th.joinable())
			th.join();
	}
}

size_t ThreadPool::GetThreadCount() const
{
	return numThreads_.load();
}

bool ThreadPool::StayAlive() const
{
	return stayAlive_.load();
}

bool ThreadPool::HasWork() const
{
	return !workQueue_.empty();
}

bool ThreadPool::ThreadsShouldProceed() const
{
	return !StayAlive() || HasWork();
}

std::future<int> ThreadPool::PostTask(std::packaged_task<int()>&& task)
{
	if (!StayAlive())
		throw std::runtime_error("Cannot post tasks on ThreadPool because it has been stopped");

	std::unique_lock<std::mutex> lock(lock_);
	workQueue_.push(std::move(task));
	threadNotifier_.notify_one();
	return workQueue_.back().get_future();
}

void ThreadPool::RunTask(ThreadPool* pool)
{
	if (!pool)
		throw std::runtime_error("ThreadPool::Work was given a nullptr value");

	while (pool->StayAlive() || pool->HasWork())
	{
		std::packaged_task<int()> task;
		{
			std::unique_lock<std::mutex> lock(pool->lock_);
			pool->threadNotifier_.wait(lock, [pool]() { return pool->ThreadsShouldProceed(); });

			if (pool->HasWork())
				task = PopWorkQueue(pool->workQueue_);
		}

		if (task.valid())
			task();
	}

	pool->numThreads_.store(pool->numThreads_ - 1);
}

