#include "IOpenMPEntity.h"

#include <stdexcept>
#include <string>

#include <omp.h>

namespace entity {

IOpenMPEntity::IOpenMPEntity(const int numThreads, const bool setDynamic, const bool setNested) :
	numThreads_(numThreads), setDynamic_(setDynamic), setNested_(setNested)
{
	if (numThreads_ < 1)
		throw std::runtime_error("Cannot start IOpenMPEntity with invalid number of threads: " + std::to_string(numThreads_));
}

IOpenMPEntity::~IOpenMPEntity() = default;

IOpenMPEntity::IOpenMPEntity(const IOpenMPEntity&) = default;
IOpenMPEntity& IOpenMPEntity::operator=(const IOpenMPEntity&) = default;
IOpenMPEntity::IOpenMPEntity(IOpenMPEntity&&) = default;
IOpenMPEntity& IOpenMPEntity::operator=(IOpenMPEntity&&) = default;

void IOpenMPEntity::Start()
{
	omp_set_num_threads(numThreads_);
	// can the implementation vary the number of threads dynamically (some impl don't support this)
	omp_set_dynamic(setDynamic_);
	// are nested parallel regions supported (some impl don't support this)
	omp_set_nested(setNested_);

	Run();
}
} // end namespace entity
