#ifndef global_iopen_mp_entity
#define global_iopen_mp_entity

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT IOpenMPEntity : public Entity
{
public:
	IOpenMPEntity(const int numThreads, const bool setDynamic, const bool setNested);
	virtual ~IOpenMPEntity();

	void Start();

	virtual void Run() = 0;

protected:
	int numThreads_; 
	bool setDynamic_;
	bool setNested_;
};

} // namespace global
#endif // global_iopen_mp_entity

