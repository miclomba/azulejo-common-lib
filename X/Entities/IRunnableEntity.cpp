#include "IRunnableEntity.h"

#include <memory>
#include <mutex>
#include <thread>

namespace entity {

IRunnableEntity::IRunnableEntity() = default;

IRunnableEntity::~IRunnableEntity()
{
	if (thread_)
		thread_->join();
}

void IRunnableEntity::Start()
{
	if (thread_)
		throw std::runtime_error("IRunnableEntity already started.");
	thread_ = std::make_shared<std::thread>(&IRunnableEntity::Run, this);
}

void IRunnableEntity::Join()
{
	if (!thread_)
		throw std::runtime_error("IRunnableEntity not started.");
	thread_->join();
	thread_.reset();
}

} // end namespace entity
