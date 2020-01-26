
#include "config.h"

#include <stdexcept>

#include <gtest/gtest.h>

#include <omp.h>

#include "Intraprocess/IOMPRunnable.h"

using intraprocess::IOMPRunnable;

namespace
{
const int EXPECTED_VALUE = 8;
const int NUM_THREADS = 1;
const bool SET_DYNAMIC = false;
const bool SET_NESTED = false;

struct Runnable : public IOMPRunnable
{
	Runnable(const size_t numThreads, const bool setDynamic, const bool setNested) :
		IOMPRunnable(numThreads,setDynamic,setNested) 
	{
	}

	Runnable(const size_t numThreads, const bool setDynamic) :
		IOMPRunnable(numThreads,setDynamic) 
	{
	}

	Runnable(const size_t numThreads) :
		IOMPRunnable(numThreads) 
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

TEST(IOMPRunnable, Construct)
{
	EXPECT_NO_THROW(Runnable(1, SET_DYNAMIC, SET_NESTED));
	EXPECT_NO_THROW(Runnable(1, SET_DYNAMIC));
	EXPECT_NO_THROW(Runnable(1));
}

TEST(IOMPRunnable, CopyConstruct)
{
	Runnable source(1, SET_DYNAMIC, SET_NESTED);
	Runnable target(source);

	EXPECT_EQ(source.GetNumThreads(), target.GetNumThreads());
	EXPECT_EQ(source.GetSetDynamic(), target.GetSetDynamic());
	EXPECT_EQ(source.GetSetNested(), target.GetSetNested());
}

TEST(IOMPRunnable, CopyAssign)
{
	Runnable source(1, SET_DYNAMIC, SET_NESTED);
	Runnable target(2, !SET_DYNAMIC, !SET_NESTED);

	target = source;

	EXPECT_EQ(source.GetNumThreads(), target.GetNumThreads());
	EXPECT_EQ(source.GetSetDynamic(), target.GetSetDynamic());
	EXPECT_EQ(source.GetSetNested(), target.GetSetNested());
}

TEST(IOMPRunnable, MoveConstruct)
{
	Runnable source(NUM_THREADS, SET_DYNAMIC, SET_NESTED);
	Runnable target(std::move(source));

	EXPECT_EQ(NUM_THREADS, target.GetNumThreads());
	EXPECT_EQ(SET_DYNAMIC, target.GetSetDynamic());
	EXPECT_EQ(SET_NESTED, target.GetSetNested());
}

TEST(IOMPRunnable, MoveAssign)
{
	Runnable source(NUM_THREADS, SET_DYNAMIC, SET_NESTED);
	Runnable target(NUM_THREADS + 1, !SET_DYNAMIC, !SET_NESTED);

	target = std::move(source);

	EXPECT_EQ(NUM_THREADS, target.GetNumThreads());
	EXPECT_EQ(SET_DYNAMIC, target.GetSetDynamic());
	EXPECT_EQ(SET_NESTED, target.GetSetNested());
}

TEST(IOMPRunnable, ConstructorThrows)
{
	EXPECT_THROW(Runnable(0, SET_DYNAMIC, SET_NESTED), std::runtime_error);
}

TEST(IOMPRunnable, Start)
{
	Runnable openmp(4,SET_DYNAMIC,SET_NESTED);
	EXPECT_NO_THROW(openmp.Start());
	EXPECT_EQ(openmp.GetSharedTotal(), EXPECTED_VALUE);
}

TEST(IOMPRunnable, GetNumThreads)
{
	Runnable openmp(4, SET_DYNAMIC, SET_NESTED);
	EXPECT_EQ(openmp.GetNumThreads(), 4);
	Runnable openmp2(4, SET_DYNAMIC);
	EXPECT_EQ(openmp2.GetNumThreads(), 4);
	Runnable openmp3(4);
	EXPECT_EQ(openmp3.GetNumThreads(), 4);
}

TEST(IOMPRunnable, GetDynamicState)
{
	Runnable openmp(4, SET_DYNAMIC, SET_NESTED);
	EXPECT_EQ(openmp.GetDynamicState(), SET_DYNAMIC);
	Runnable openmp2(4, SET_DYNAMIC);
	EXPECT_EQ(openmp2.GetDynamicState(), SET_DYNAMIC);
	Runnable openmp3(4);
	EXPECT_EQ(openmp3.GetDynamicState(), !SET_DYNAMIC);
}

TEST(IOMPRunnable, GetNestedState)
{
	Runnable openmp(4, SET_DYNAMIC, SET_NESTED);
	EXPECT_EQ(openmp.GetNestedState(), SET_NESTED);
	Runnable openmp2(4, SET_DYNAMIC);
	EXPECT_EQ(openmp2.GetNestedState(), !SET_NESTED);
	Runnable openmp3(4);
	EXPECT_EQ(openmp3.GetNestedState(), !SET_NESTED);
}
