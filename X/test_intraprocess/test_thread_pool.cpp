
#include "config.h"

#include <chrono>
#include <future>
#include <memory>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/asio/thread_pool.hpp>

#include "Intraprocess/ThreadPool.h"

#define REPEAT_BEGIN for (size_t i = 0; i < REPEAT; ++i) {
#define REPEAT_END }

using intraprocess::ThreadPool;

namespace
{
const int INVALID_VAL = 0;
const int VALID_VAL = 7;
const size_t REPEAT = 10;
const size_t ONE_THREAD = 1;
const size_t TWO_THREADS = 2;
const size_t EIGHT_THREADS = 8;
const size_t ZERO_THREADS = 0;
const size_t ZERO_TASKS = 0;
const size_t ONE_TASK = 1;
const size_t TWO_TASKS = 2;
std::chrono::milliseconds HUNDRED_MSEC(100);

struct ThreadPoolF : public testing::Test
{
	std::function<void(int&)> SleepAndSetResourceValue = [this](int& val)
	{
		std::this_thread::sleep_for(HUNDRED_MSEC);
		value_ += val;
	};

	int GetResourceValue() { return value_; }
	void SetResourceValue(const int val) { value_ = val; }

	int VALID_VAL_PARAM{VALID_VAL};
private:
	int value_{ 0 };
};
} // end namespace anonymous

TEST(ThreadPool, Construct)
{
	REPEAT_BEGIN
	EXPECT_NO_THROW(ThreadPool pool(TWO_THREADS));
	REPEAT_END
}

TEST(ThreadPool, DefaultConstruct)
{
	REPEAT_BEGIN
	EXPECT_NO_THROW(ThreadPool pool);
	REPEAT_END
}

TEST(ThreadPool, ConstructThrows)
{
	REPEAT_BEGIN
	EXPECT_THROW(ThreadPool pool(ZERO_THREADS), std::runtime_error);
	REPEAT_END
}

TEST_F(ThreadPoolF, Destruct)
{
	REPEAT_BEGIN
	SetResourceValue(INVALID_VAL);
	{
		ThreadPool pool(ONE_THREAD);
		EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);
		EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);

		pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);
	}
	EXPECT_EQ(GetResourceValue(), VALID_VAL);

	REPEAT_END
}

TEST(ThreadPool, GetTaskCount)
{
	REPEAT_BEGIN
	ThreadPool pool(ONE_THREAD);
	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);
	REPEAT_END
}

TEST(ThreadPool, GetThreadCount)
{
	REPEAT_BEGIN
	{
		ThreadPool pool(ONE_THREAD);
		EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);
	}
	{
		ThreadPool pool(EIGHT_THREADS);
		EXPECT_EQ(pool.GetThreadCount(), EIGHT_THREADS);
	}
	{
		ThreadPool pool;
		EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);
	}
	REPEAT_END
}

TEST_F(ThreadPoolF, Join)
{
	bool hasTasksAfterPost = false;

	REPEAT_BEGIN
	SetResourceValue(INVALID_VAL);

	ThreadPool pool(TWO_THREADS);
	EXPECT_EQ(pool.GetThreadCount(), TWO_THREADS);
	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);

	pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);
	if (pool.GetTaskCount() > ZERO_TASKS)
		hasTasksAfterPost = true;

	EXPECT_NO_THROW(pool.Join());
	EXPECT_EQ(pool.GetThreadCount(), ZERO_THREADS);
	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);
	EXPECT_EQ(GetResourceValue(), VALID_VAL);
	REPEAT_END

	EXPECT_TRUE(hasTasksAfterPost);
}

TEST_F(ThreadPoolF, Stop)
{
	bool hasTasksAfterStop = false;

	REPEAT_BEGIN
	ThreadPool pool(ONE_THREAD);
	EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);
	pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);
	pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);
	pool.Stop();
	EXPECT_EQ(pool.GetThreadCount(), ZERO_THREADS);
	if (pool.GetTaskCount() > ZERO_TASKS)
		hasTasksAfterStop = true;

	REPEAT_END

	EXPECT_TRUE(hasTasksAfterStop);
}

TEST_F(ThreadPoolF, Post)
{
	bool hasTasksAfterPost = false;

	REPEAT_BEGIN
	SetResourceValue(INVALID_VAL);

	ThreadPool pool(ONE_THREAD);
	EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);
	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);

	pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);
	if (pool.GetTaskCount() > ZERO_TASKS)
		hasTasksAfterPost = true;

	EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);

	pool.Join();

	EXPECT_EQ(GetResourceValue(), VALID_VAL);
	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);
	REPEAT_END

	EXPECT_TRUE(hasTasksAfterPost);
}

TEST_F(ThreadPoolF, PostOneTaskUsingEightThreads)
{
	REPEAT_BEGIN
	SetResourceValue(INVALID_VAL);

	ThreadPool pool(EIGHT_THREADS);
	EXPECT_EQ(pool.GetThreadCount(), EIGHT_THREADS);
	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);

	pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);
	EXPECT_EQ(pool.GetThreadCount(), EIGHT_THREADS);

	pool.Join();

	EXPECT_EQ(GetResourceValue(), VALID_VAL);
	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);
	REPEAT_END
}

TEST_F(ThreadPoolF, PostEightTaskUsingOneThreads)
{
	REPEAT_BEGIN
	SetResourceValue(INVALID_VAL);

	ThreadPool pool(ONE_THREAD);
	EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);
	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);

	const size_t EIGHT_TASKS = 8;
	for (size_t i = 0; i < EIGHT_TASKS; ++i)
		pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);

	EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);

	pool.Join();

	EXPECT_EQ(GetResourceValue(), EIGHT_TASKS * VALID_VAL);

	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);
	REPEAT_END
}

TEST_F(ThreadPoolF, PostAfterStopThrows)
{
	bool hasTasksAfterStop = false;

	REPEAT_BEGIN
	ThreadPool pool(ONE_THREAD);
	EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);
	pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);
	pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);
	pool.Stop();
	EXPECT_EQ(pool.GetThreadCount(), ZERO_THREADS);
	if (pool.GetTaskCount() > ZERO_TASKS)
		hasTasksAfterStop = true;

	EXPECT_THROW(pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM), std::runtime_error);
	REPEAT_END

	EXPECT_TRUE(hasTasksAfterStop);
}

TEST_F(ThreadPoolF, PostAfterJoinThrows)
{
	bool hasTasksAfterPost = false;

	REPEAT_BEGIN
	ThreadPool pool(ONE_THREAD);
	EXPECT_EQ(pool.GetThreadCount(), ONE_THREAD);
	pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);
	pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM);

	if (pool.GetTaskCount() > ZERO_TASKS)
		hasTasksAfterPost = true;

	pool.Join();
	EXPECT_EQ(pool.GetThreadCount(), ZERO_THREADS);
	EXPECT_EQ(pool.GetTaskCount(), ZERO_TASKS);

	EXPECT_THROW(pool.Post(SleepAndSetResourceValue, VALID_VAL_PARAM), std::runtime_error);
	REPEAT_END

	EXPECT_TRUE(hasTasksAfterPost);
}

/*
namespace
{
boost::asio::io_service io_service;

void WorkerThread()
{
	std::cout << "Thread Start\n";
	io_service.run();
	std::cout << "Thread Finish\n";
}
}

TEST(Foo)
{
	boost::shared_ptr< boost::asio::io_service::work > work(new boost::asio::io_service::work(io_service));

	std::cout << "Press [return] to exit." << std::endl;

	boost::thread_group worker_threads;
	for (int x = 0; x < 4; ++x)
	{
		worker_threads.create_thread(WorkerThread);
	}

	std::cin.get();

	io_service.stop();

	worker_threads.join_all();

	boost::asio::thread_pool pool;
	boost::asio::post(pool, []() {});
}
*/
#undef REPEAT_BEGIN 
#undef REPEAT_END

