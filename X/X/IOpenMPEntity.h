#ifndef entity_iopenmpentity_h
#define entity_iopenmpentity_h

#include "config.h"
#include "Entity.h"

namespace entity {

class ENTITY_DLL_EXPORT IOpenMPEntity : public Entity
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

} // namespace entity
#endif // entity_iopenmpentity_h

