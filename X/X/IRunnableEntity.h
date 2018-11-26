#ifndef global_irunnable_entity
#define global_irunnable_entity

#include <memory>
#include <mutex>
#include <thread>

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT IRunnableEntity : public Entity
{
public:
	virtual ~IRunnableEntity();

	void Start();
	void Join();

protected:
	virtual void Run() = 0;

private:
	std::mutex mtx_;

	std::shared_ptr<std::thread> thread_;
};

} // namespace global

#endif // global_irunnable_entity
