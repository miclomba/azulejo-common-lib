#include "IOpenMP.h"

#include <stdexcept>
#include <string>

#include <omp.h>

using intraprocess::IOpenMP;

IOpenMP::IOpenMP(const int numThreads, const bool setDynamic, const bool setNested) :
	numThreads_(numThreads), setDynamic_(setDynamic), setNested_(setNested)
{
	if (numThreads_ < 1)
		throw std::runtime_error("Cannot start IOpenMP with invalid number of threads: " + std::to_string(numThreads_));
}

IOpenMP::~IOpenMP() = default;

IOpenMP::IOpenMP(const IOpenMP&) = default;
IOpenMP& IOpenMP::operator=(const IOpenMP&) = default;
IOpenMP::IOpenMP(IOpenMP&&) = default;
IOpenMP& IOpenMP::operator=(IOpenMP&&) = default;

void IOpenMP::Start()
{
	omp_set_num_threads(numThreads_);
	// can the implementation vary the number of threads dynamically (some impl don't support this)
	omp_set_dynamic(setDynamic_);
	// are nested parallel regions supported (some impl don't support this)
	omp_set_nested(setNested_);

	Run();
}
