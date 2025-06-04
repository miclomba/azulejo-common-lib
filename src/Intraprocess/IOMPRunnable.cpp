#include "Intraprocess/IOMPRunnable.h"

#include <stdexcept>
#include <string>

#include <omp.h>

using intraprocess::IOMPRunnable;

IOMPRunnable::IOMPRunnable(const size_t numThreads, const bool setDynamic, const bool setNested) : numThreads_(numThreads), setDynamic_(setDynamic), setNested_(setNested)
{
	if (numThreads_ == 0)
		throw std::runtime_error("Cannot start IOMPRunnable with invalid number of threads: " + std::to_string(numThreads_));
}

IOMPRunnable::~IOMPRunnable() = default;

IOMPRunnable::IOMPRunnable(const IOMPRunnable &) = default;
IOMPRunnable &IOMPRunnable::operator=(const IOMPRunnable &) = default;
IOMPRunnable::IOMPRunnable(IOMPRunnable &&) = default;
IOMPRunnable &IOMPRunnable::operator=(IOMPRunnable &&) = default;

void IOMPRunnable::Start()
{
	omp_set_num_threads(numThreads_);
	// can the implementation vary the number of threads dynamically (some impl don't support this)
	omp_set_dynamic(setDynamic_);
	// are nested parallel regions supported (some impl don't support this)
	omp_set_nested(setNested_);

	Run();
}

size_t IOMPRunnable::GetNumThreads() const
{
	return numThreads_;
}

bool IOMPRunnable::GetDynamicState() const
{
	return setDynamic_;
}

bool IOMPRunnable::GetNestedState() const
{
	return setNested_;
}
