
#include "config.h"

#include <stdexcept>

#include <gtest/gtest.h>

#include <omp.h>

#include "Entities/IOpenMPEntity.h"

using entity::IOpenMPEntity;

namespace
{
const int EXPECTED_VALUE = 8;

struct Runnable : public IOpenMPEntity
{
	Runnable(const int numThreads, const bool setDynamic, const bool setNested) :
		IOpenMPEntity(numThreads,setDynamic,setNested) 
	{
	}

	int GetSharedTotal()  { return sharedTotal_; }

protected:
	void Run() override 
	{
		// Create an atomic
		#pragma omp atomic
		int sharedAtomic = 0;

		int sharedMaster = 0;
		int sharedCritical = 0;
		int sharedSingle = 0;

		#pragma omp parallel shared(sharedAtomic, sharedMaster, sharedCritical, sharedSingle)
		{
			// only the master thread allowed here
			#pragma omp master
			{
				sharedMaster = omp_get_thread_num();
				sharedMaster += 1;
			}

			// divide the loop into threads
			#pragma omp for schedule(auto)
			for (int i = 0; i < numThreads_; ++i)
				sharedAtomic += 1;

			// divide the work into sections (one per thread) 
			#pragma omp sections
			{
				#pragma omp section
				{
					// critical section A
					#pragma omp critical A
					{
						sharedCritical += 1;
					}
				}
				#pragma omp section
				{
					// critical section B
					#pragma omp critical B
					{
						sharedCritical += 1;
					}
				}
				#pragma omp barrier
			}

			// for this to be executed by only one thread
			#pragma omp single
			{
				sharedSingle += 1;
			}

			// Flush all thread CPU registers (hardware buffers) back to memory for consistency between threads
			#pragma omp flush

			sharedTotal_ = sharedAtomic + sharedCritical + sharedSingle + sharedMaster;
		}
	};

private:
	int sharedTotal_{ 0 };
};
} // end namespace anonymous

TEST(IOpenMPEntity, ConstructorThrows)
{
	EXPECT_THROW(Runnable(-4, false, false), std::runtime_error);
}

TEST(IOpenMPEntity, Start)
{
	Runnable openmp(4,false,false);
	EXPECT_NO_THROW(openmp.Start());
	EXPECT_EQ(openmp.GetSharedTotal(), EXPECTED_VALUE);
}
