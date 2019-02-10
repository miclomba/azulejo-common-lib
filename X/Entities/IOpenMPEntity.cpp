#include "IOpenMPEntity.h"

#include <omp.h>

namespace entity {

IOpenMPEntity::IOpenMPEntity(const int numThreads, const bool setDynamic, const bool setNested) :
	numThreads_(numThreads), setDynamic_(setDynamic), setNested_(setNested)
{
}

IOpenMPEntity::~IOpenMPEntity() 
{
}

void IOpenMPEntity::Start()
{
	omp_set_num_threads(numThreads_);
	omp_set_dynamic(setDynamic_);
	omp_set_nested(setNested_);

	Run();
}

} // end namespace entity
