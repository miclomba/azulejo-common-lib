#include "ISharedMemory.h"

#include <memory>
#include <stdexcept>
#include <string>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

using interprocess::ISharedMemory;

ISharedMemory::ISharedMemory() = default;

ISharedMemory::~ISharedMemory()
{
	if (!shmem_ || !isShmemOwner_)
		return;

	const char* name = shmem_->get_name();
	boost::interprocess::shared_memory_object::remove(name);
}

bool ISharedMemory::Destroy()
{
	if (!shmem_ || !isShmemOwner_)
		return true;

	const char* name = shmem_->get_name();
	bool destroyed = boost::interprocess::shared_memory_object::remove(name);

	shmem_ = nullptr;
	isShmemOwner_ = false;
	return destroyed;
}

void ISharedMemory::Create(const std::string& name, const size_t size)
{
	using namespace boost::interprocess;

	if (size <= 0)
		throw std::runtime_error("Shared memory cannot be allocated with an invalid size: " + std::to_string(size));
	if (name.empty())
		throw std::runtime_error("Shared memory cannot be allocated without a name");
	if (shmem_)
		throw std::runtime_error("Shared memory already allocated using name=" + std::string(shmem_->get_name()));

	try
	{
		shmem_ = std::make_shared<shared_memory_object>(create_only, name.c_str(), read_write);
	}
	catch (std::exception&)
	{
		throw std::runtime_error("Shared memory cannot be allocated because " + name + " is already allocated");
	}

	size_t currentSize = GetSharedSize();
	offset_t desiredSize = static_cast<offset_t>(size);

	if (desiredSize > currentSize)
		shmem_->truncate(desiredSize);

	isShmemOwner_ = true;
}

void ISharedMemory::Open(const std::string& name)
{
	using namespace boost::interprocess;

	if (shmem_)
		throw std::runtime_error("Shared memory already allocated using name=" + std::string(shmem_->get_name()));

	try
	{
		shmem_ = std::make_shared<shared_memory_object>(open_only, name.c_str(), read_write);
	}
	catch (std::exception&)
	{
		throw std::runtime_error("Shared memory cannot be allocated because " + name + " is already allocated");
	}
}

std::string ISharedMemory::GetSharedName() const
{
	if (!shmem_)
		throw std::runtime_error("Cannot get shared memory name because shared memory is not allocated.");
	return shmem_->get_name();
}

size_t ISharedMemory::GetSharedSize() const
{
	if (!shmem_)
		throw std::runtime_error("Cannot get shared memory size because shared memory is not allocated.");

	boost::interprocess::offset_t size;
	shmem_->get_size(size);

	return static_cast<size_t>(size);
}

void* ISharedMemory::GetSharedAddress() const
{
	using namespace boost::interprocess;

	if (!shmem_)
		throw std::runtime_error("Cannot get shared memory address because shared memory is not allocated.");

	mapped_region region{ *shmem_, read_write };
	return static_cast<void*>(region.get_address());
}

bool ISharedMemory::IsSharedMemoryOwner() const
{
	return isShmemOwner_;
}
