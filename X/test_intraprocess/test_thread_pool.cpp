
#include "config.h"

#include <chrono>
#include <future>
#include <memory>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include "Intraprocess/ThreadPool.h"

using intraprocess::ThreadPool;

namespace
{
const int FUTURE_VALUE = 7;
const size_t ONE_THREAD = 1;
const size_t TWO_THREADS = 2;
const size_t EIGHT_THREADS = 8;
const size_t INVALID_THREAD_COUNT = 0;
std::chrono::seconds TWO_SEC(2);
} // end namespace anonymous

TEST(ThreadPool, Construct)
{
	EXPECT_NO_THROW(ThreadPool pool(TWO_THREADS));
}

TEST(ThreadPool, ConstructThrows)
{
	EXPECT_THROW(ThreadPool pool(INVALID_THREAD_COUNT), std::runtime_error);
}

TEST(ThreadPool, Destruct)
{
	std::future<int> futuro;

	{
		ThreadPool pool(ONE_THREAD);

		auto lambda = []() 
		{
			std::this_thread::sleep_for(TWO_SEC);
			return FUTURE_VALUE; 
		};
		futuro = pool.PostTask(std::packaged_task<int()>(lambda));
	}

	EXPECT_TRUE(futuro.valid());
	EXPECT_EQ(futuro.get(), FUTURE_VALUE);
}

TEST(ThreadPool, GetThreadCount)
{
	ThreadPool pool(TWO_THREADS);
	EXPECT_EQ(pool.GetThreadCount(), TWO_THREADS);
}

TEST(ThreadPool, Shutdown)
{
	ThreadPool pool(TWO_THREADS);
	EXPECT_EQ(pool.GetThreadCount(), TWO_THREADS);

	auto lambda = []()
	{
		std::this_thread::sleep_for(TWO_SEC);
		return FUTURE_VALUE;
	};
	std::future<int> futuro = pool.PostTask(std::packaged_task<int()>(lambda));

	EXPECT_NO_THROW(pool.Shutdown());
	EXPECT_EQ(pool.GetThreadCount(), INVALID_THREAD_COUNT);
	EXPECT_TRUE(futuro.valid());
	EXPECT_EQ(futuro.get(), FUTURE_VALUE);
}

TEST(ThreadPool, PostOneTaskUsingOneThread)
{
	ThreadPool pool(ONE_THREAD);

	auto lambda = []() { return FUTURE_VALUE; };
	std::future<int> futuro = pool.PostTask(std::packaged_task<int()>(lambda));
	EXPECT_TRUE(futuro.valid());
	EXPECT_EQ(futuro.get(), FUTURE_VALUE);
}

TEST(ThreadPool, PostOneTaskUsingTwoThreads)
{
	ThreadPool pool(EIGHT_THREADS);

	auto lambda = []() { return FUTURE_VALUE; };
	std::future<int> futuro = pool.PostTask(std::packaged_task<int()>(lambda));
	EXPECT_TRUE(futuro.valid());
	EXPECT_EQ(futuro.get(), FUTURE_VALUE);
}

TEST(ThreadPool, PostEightTaskUsingOneThreads)
{
	ThreadPool pool(ONE_THREAD);

	auto lambda = []() { return FUTURE_VALUE; };
	auto lambda2 = []() { return FUTURE_VALUE; };
	auto lambda3 = []() { return FUTURE_VALUE; };
	auto lambda4 = []() { return FUTURE_VALUE; };
	auto lambda5 = []() { return FUTURE_VALUE; };
	auto lambda6 = []() { return FUTURE_VALUE; };
	auto lambda7 = []() { return FUTURE_VALUE; };
	auto lambda8 = []() { return FUTURE_VALUE; };

	std::future<int> futuro = pool.PostTask(std::packaged_task<int()>(lambda));
	std::future<int> futuro2 = pool.PostTask(std::packaged_task<int()>(lambda2));
	std::future<int> futuro3 = pool.PostTask(std::packaged_task<int()>(lambda2));
	std::future<int> futuro4 = pool.PostTask(std::packaged_task<int()>(lambda2));
	std::future<int> futuro5 = pool.PostTask(std::packaged_task<int()>(lambda2));
	std::future<int> futuro6 = pool.PostTask(std::packaged_task<int()>(lambda2));
	std::future<int> futuro7 = pool.PostTask(std::packaged_task<int()>(lambda2));
	std::future<int> futuro8 = pool.PostTask(std::packaged_task<int()>(lambda2));

	EXPECT_TRUE(futuro.valid());
	EXPECT_TRUE(futuro2.valid());
	EXPECT_TRUE(futuro3.valid());
	EXPECT_TRUE(futuro4.valid());
	EXPECT_TRUE(futuro5.valid());
	EXPECT_TRUE(futuro6.valid());
	EXPECT_TRUE(futuro7.valid());
	EXPECT_TRUE(futuro8.valid());

	EXPECT_EQ(futuro.get(), FUTURE_VALUE);
	EXPECT_EQ(futuro2.get(), FUTURE_VALUE);
	EXPECT_EQ(futuro3.get(), FUTURE_VALUE);
	EXPECT_EQ(futuro4.get(), FUTURE_VALUE);
	EXPECT_EQ(futuro5.get(), FUTURE_VALUE);
	EXPECT_EQ(futuro6.get(), FUTURE_VALUE);
	EXPECT_EQ(futuro7.get(), FUTURE_VALUE);
	EXPECT_EQ(futuro8.get(), FUTURE_VALUE);
}

TEST(ThreadPool, PostTaskThrows)
{
	ThreadPool pool(EIGHT_THREADS);
	pool.Shutdown();

	auto lambda = []() { return FUTURE_VALUE; };
	EXPECT_THROW(pool.PostTask(std::packaged_task<int()>(lambda)), std::runtime_error);
}


