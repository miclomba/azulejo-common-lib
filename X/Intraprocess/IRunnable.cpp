#include "IRunnable.h"

#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>

using intraprocess::IRunnable;

IRunnable::IRunnable() = default;

IRunnable::~IRunnable()
{
	if (thread_)
		thread_->join();
}

void IRunnable::Start()
{
	if (thread_)
		throw std::runtime_error("IRunnable already started.");
	thread_ = std::make_shared<std::thread>(&IRunnable::Run, this);
}

void IRunnable::Join()
{
	if (!thread_)
		throw std::runtime_error("IRunnable not started.");
	thread_->join();
	thread_.reset();
}
