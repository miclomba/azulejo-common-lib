#ifndef entity_irunnableentity_h
#define entity_irunnableentity_h

#include <memory>
#include <mutex>
#include <thread>

#include "config.h"
#include "Entity.h"

namespace entity {

class ENTITY_DLL_EXPORT IRunnableEntity : public Entity
{
public:
	IRunnableEntity();
	virtual ~IRunnableEntity();

	IRunnableEntity(const IRunnableEntity&) = delete;
	IRunnableEntity& operator=(const IRunnableEntity&) = delete;
	IRunnableEntity(IRunnableEntity&&) = delete;
	IRunnableEntity& operator=(IRunnableEntity&&) = delete;

	void Start();
	void Join();

protected:
	virtual void Run() = 0;

private:
	std::mutex mtx_;

	std::shared_ptr<std::thread> thread_;
};

} // namespace entity

#endif // entity_irunnableentity_h
