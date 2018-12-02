#include "IShareableEntity.h"

#include <memory>
#include <string>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace global {

IShareableEntity::~IShareableEntity()
{
	if (shmem_ && isShmemOwner_)
	{
		auto name = shmem_->get_name();
		bool removed = boost::interprocess::shared_memory_object::remove(name);
		if (!removed)
			throw std::runtime_error("Could not remove shared memory: name=" + std::string(name));
	}
}

void IShareableEntity::Create(const std::string& name, const size_t size)
{
	using namespace boost::interprocess;

	if (shmem_)
		throw std::runtime_error("Shared memory already allocated using name=" + std::string(shmem_->get_name()));

	shmem_ = std::make_shared<shared_memory_object>(create_only, name.c_str(), read_write);

	auto currentSize = GetSharedSize();
	auto desiredSize = static_cast<offset_t>(size);

	if (desiredSize > currentSize)
		shmem_->truncate(desiredSize);

	isShmemOwner_ = true;
}

void IShareableEntity::Open(const std::string& name)
{
	using namespace boost::interprocess;

	if (shmem_)
		throw std::runtime_error("Shared memory already allocated using name=" + std::string(shmem_->get_name()));

	shmem_ = std::make_shared<shared_memory_object>(open_only, name.c_str(), read_write);
}

std::string IShareableEntity::GetSharedName() const
{
	if (!shmem_)
		throw std::runtime_error("Cannot get shared memory name because shared memory is not allocated.");
	return shmem_->get_name();
}

size_t IShareableEntity::GetSharedSize() const
{
	if (!shmem_)
		throw std::runtime_error("Cannot get shared memory size because shared memory is not allocated.");

	boost::interprocess::offset_t size;
	shmem_->get_size(size);

	return static_cast<size_t>(size);
}

void* IShareableEntity::GetSharedAddress() const
{
	using namespace boost::interprocess;

	if (!shmem_)
		throw std::runtime_error("Cannot get shared memory address because shared memory is not allocated.");

	mapped_region region{ *shmem_, read_write };
	return static_cast<void*>(region.get_address());
}

}
