
#include "config.h"

#include <stdexcept>

#include <gtest/gtest.h>

#include <omp.h>

#include "Entities/IOpenMPEntity.h"

using entity::IOpenMPEntity;

namespace
{
const int EXPECTED_VALUE = 8;
const int NUM_THREADS = 1;
const bool SET_DYNAMIC = false;
const bool SET_NESTED = false;

struct Runnable : public IOpenMPEntity
{
	Runnable(const int numThreads, const bool setDynamic, const bool setNested) :
		IOpenMPEntity(numThreads,setDynamic,setNested) 
	{
	}

	int GetSharedTotal()  { return sharedTotal_; }
	int GetNumThreads() { return numThreads_; };
	bool GetSetDynamic() { return setDynamic_; };
	bool GetSetNested() { return setNested_; };

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

TEST(IOpenMPEntity, Construct)
{
	EXPECT_NO_THROW(Runnable(1, false, false));
}

TEST(IOpenMPEntity, CopyConstruct)
{
	Runnable runnable(1, false, false);
	Runnable copy(runnable);

	EXPECT_EQ(runnable.GetNumThreads(), copy.GetNumThreads());
	EXPECT_EQ(runnable.GetSetDynamic(), copy.GetSetDynamic());
	EXPECT_EQ(runnable.GetSetNested(), copy.GetSetNested());
}

TEST(IOpenMPEntity, CopyAssign)
{
	Runnable runnable(1, false, false);
	Runnable copy(2, true, true);

	copy = runnable;

	EXPECT_EQ(runnable.GetNumThreads(), copy.GetNumThreads());
	EXPECT_EQ(runnable.GetSetDynamic(), copy.GetSetDynamic());
	EXPECT_EQ(runnable.GetSetNested(), copy.GetSetNested());
}

TEST(IOpenMPEntity, MoveConstruct)
{
	Runnable runnable(NUM_THREADS, SET_DYNAMIC, SET_NESTED);
	Runnable copy(std::move(runnable));

	EXPECT_EQ(NUM_THREADS, copy.GetNumThreads());
	EXPECT_EQ(SET_DYNAMIC, copy.GetSetDynamic());
	EXPECT_EQ(SET_NESTED, copy.GetSetNested());
}

TEST(IOpenMPEntity, MoveAssign)
{
	Runnable runnable(NUM_THREADS, SET_DYNAMIC, SET_NESTED);
	Runnable copy(NUM_THREADS + 1, !SET_DYNAMIC, !SET_NESTED);

	copy = std::move(runnable);

	EXPECT_EQ(NUM_THREADS, copy.GetNumThreads());
	EXPECT_EQ(SET_DYNAMIC, copy.GetSetDynamic());
	EXPECT_EQ(SET_NESTED, copy.GetSetNested());
}

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
