#ifndef entity_isharedmemoryentity_h
#define entity_isharedmemoryentity_h

#include <memory>
#include <string>

#include <boost/interprocess/shared_memory_object.hpp>

#include "config.h"
#include "Entity.h"

namespace entity {

class ENTITY_DLL_EXPORT ISharedMemoryEntity : public Entity
{
public:
	virtual ~ISharedMemoryEntity();

	std::string GetSharedName() const;
	size_t GetSharedSize() const;
	void* GetSharedAddress() const;

	void Create(const std::string& name, const size_t size);
	void Open(const std::string& name);

private:
	std::shared_ptr<boost::interprocess::shared_memory_object> shmem_;
	
	// TODO : find a way to remove this	
	bool isShmemOwner_{ false };
};

} // end namespace entity

#endif // entity_isharedmemoryentity_h
