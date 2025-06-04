
template<typename T, typename... Params, typename... Args>
void ThreadPool::Post(std::function<T(Params&...)> task, Args&&... args)
{
	if (!KeepRunning() || Die())
		throw std::runtime_error("Cannot post tasks on ThreadPool because it has been stopped");

	std::unique_lock<std::mutex> lock(lock_);

	std::packaged_task<void()> wrappedTask(
		[task, &args...]() { task(std::forward<Args>(args)...); }
	);
	workQueue_.push(std::move(wrappedTask));
	threadNotifier_.notify_one();
}
