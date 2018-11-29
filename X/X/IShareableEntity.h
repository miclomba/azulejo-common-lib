#ifndef global_ishareable_entity
#define global_ishareable_entity

#include <memory>
#include <string>

#include <boost/interprocess/shared_memory_object.hpp>

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT IShareableEntity : public Entity
{
public:
	virtual ~IShareableEntity();

	virtual void Share(const std::string& name) = 0;
	
	std::string GetSharedName() const;
	size_t GetSharedSize() const;
	void* GetSharedAddress() const;

protected:
	void OpenOrCreate(const std::string& name, const size_t size);

private:
	std::shared_ptr<boost::interprocess::shared_memory_object> shmem_;
};

} // namespace global

#endif // global_ishareable_entity
