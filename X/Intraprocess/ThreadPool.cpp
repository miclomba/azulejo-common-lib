#include "ThreadPool.h"

#include <functional>
#include <future>
#include <mutex>
#include <stdexcept>
#include <thread>

using intraprocess::ThreadPool;

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
	Shutdown();
}

void ThreadPool::Shutdown()
{
	stayAlive_ = false;
	for (std::thread& th : workerThreads_)
	{
		if (th.joinable())
			th.join();
	}
	workerThreads_.clear();
}

size_t ThreadPool::GetThreadCount() const
{
	return workerThreads_.size();
}

std::future<int> ThreadPool::PostTask(std::packaged_task<int()>&& task)
{
	std::unique_lock<std::mutex> lock(lock_);
	workQueue_.push(std::move(task));
	return workQueue_.back().get_future();
}

void ThreadPool::RunTask(ThreadPool* pool)
{
	if (!pool)
		throw std::runtime_error("ThreadPool::Work was given a nullptr value");

	while (pool->stayAlive_ || !pool->workQueue_.empty())
	{
		std::packaged_task<int()> task;
		{
			std::unique_lock<std::mutex> lock(pool->lock_);

			if (!pool->workQueue_.empty())
			{
				task = std::move(pool->workQueue_.front());
				pool->workQueue_.pop();
			}
		}

		if (task.valid())
			task();
	}

	--(pool->numThreads_);
}

