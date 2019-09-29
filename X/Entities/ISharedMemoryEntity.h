#ifndef entity_isharedmemoryentity_h
#define entity_isharedmemoryentity_h

#include <memory>
#include <string>

#include "config.h"

#include <boost/interprocess/shared_memory_object.hpp>

#include "Entity.h"

namespace entity {

class ENTITY_DLL_EXPORT ISharedMemoryEntity : public Entity
{
public:
	ISharedMemoryEntity();
	virtual ~ISharedMemoryEntity();

	ISharedMemoryEntity(const ISharedMemoryEntity&) = delete;
	ISharedMemoryEntity& operator=(const ISharedMemoryEntity&) = delete;
	ISharedMemoryEntity(ISharedMemoryEntity&&) = delete;
	ISharedMemoryEntity& operator=(ISharedMemoryEntity&&) = delete;

	std::string GetSharedName() const;
	size_t GetSharedSize() const;
	void* GetSharedAddress() const;
	bool IsSharedMemoryOwner() const;

	void Create(const std::string& name, const size_t size);
	void Open(const std::string& name);

private:
	std::shared_ptr<boost::interprocess::shared_memory_object> shmem_;
	
	// TODO : find a way to remove this	
	bool isShmemOwner_{ false };
};

} // end namespace entity

#endif // entity_isharedmemoryentity_h
