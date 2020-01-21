
#define TEMPLATE_T template<typename T>
#define ThreadPool_t ThreadPool<T>

namespace
{
TEMPLATE_T
std::packaged_task<T()> PopWorkQueue(std::queue<std::packaged_task<T()>>& workQueue)
{
	std::packaged_task<T()> task;
	task = std::move(workQueue.front());
	workQueue.pop();
	return task;
}
} // end namespace

TEMPLATE_T
ThreadPool_t::ThreadPool(const size_t numThreads)
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

TEMPLATE_T
ThreadPool_t::~ThreadPool()
{
	Stop();
}

TEMPLATE_T
void ThreadPool_t::Stop()
{
	{
		std::unique_lock<std::mutex> lock(lock_);
		stayAlive_.store(false);
		threadNotifier_.notify_all();
	}

	while (GetThreadCount() > 0)
		continue;

	Join();
}

TEMPLATE_T
void ThreadPool_t::Join()
{
	for (std::thread& th : workerThreads_)
	{
		if (th.joinable())
			th.join();
	}
}

TEMPLATE_T
size_t ThreadPool_t::GetThreadCount() const
{
	return numThreads_.load();
}

TEMPLATE_T
size_t ThreadPool_t::GetTaskCount()
{
	std::unique_lock<std::mutex> lock(lock_);
	size_t count = workQueue_.size();
	threadNotifier_.notify_one();
	return count;
}

TEMPLATE_T
bool ThreadPool_t::StayAlive() const
{
	return stayAlive_.load();
}

TEMPLATE_T
bool ThreadPool_t::HasWorkThreadSafe()
{
	std::unique_lock<std::mutex> lock(lock_);
	return HasWorkThreadUnsafe();
}

TEMPLATE_T
bool ThreadPool_t::HasWorkThreadUnsafe() const
{
	return !workQueue_.empty();
}

TEMPLATE_T
bool ThreadPool_t::ThreadsShouldProceed()
{
	return !StayAlive() || HasWorkThreadUnsafe();
}

TEMPLATE_T
void ThreadPool_t::AddToThreadCounter(const size_t val)
{
	std::unique_lock<std::mutex> lock(lock_);
	numThreads_.store(numThreads_.load() + val);
}

TEMPLATE_T
void ThreadPool_t::RunTask(ThreadPool* pool)
{
	if (!pool)
		throw std::runtime_error("ThreadPool::Work was given a nullptr value");

	pool->AddToThreadCounter(1);

	while (pool->StayAlive() || pool->HasWorkThreadSafe())
	{
		std::packaged_task<T()> task;
		{
			std::unique_lock<std::mutex> lock(pool->lock_);
			pool->threadNotifier_.wait(lock, [pool]() { return pool->ThreadsShouldProceed(); });

			// check because the wake operation can wake up spuriously due to non-safe predicate
			if (pool->HasWorkThreadUnsafe())
				task = PopWorkQueue(pool->workQueue_);
		}

		if (task.valid())
			task();
	}

	pool->AddToThreadCounter(-1);
}

TEMPLATE_T
std::future<T> ThreadPool_t::PostTask(std::packaged_task<T()>&& task)
{
	if (!StayAlive())
		throw std::runtime_error("Cannot post tasks on ThreadPool because it has been stopped");

	std::unique_lock<std::mutex> lock(lock_);
	workQueue_.push(std::move(task));
	threadNotifier_.notify_one();
	return workQueue_.back().get_future();
}

#undef TEMPLATE_T
