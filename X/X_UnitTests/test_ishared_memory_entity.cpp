
#include "config.h"

#include <gtest/gtest.h>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

#include "X/ISharedMemoryEntity.h"

namespace
{
const std::string NAME = "shmem";
const size_t SIZE = 1024;

class ShareableCreator : public global::ISharedMemoryEntity
{
};

class ShareableAccessor : public global::ISharedMemoryEntity
{
};
}
/*
TEST(ISharedMemoryEntity, OpenOrCreate)
{
	ShareableCreator shareable;

	EXPECT_NO_THROW(shareable.Create(NAME,SIZE));
}

TEST(ISharedMemoryEntity, ThrowOnOpenOrCreate)
{
	ShareableCreator shareable;

	EXPECT_NO_THROW(shareable.Create(NAME,SIZE));
	EXPECT_THROW(shareable.Create(NAME, SIZE), std::runtime_error);
}

TEST(ISharedMemoryEntity, GetSharedName)
{
	ShareableCreator shareable;

	shareable.Create(NAME,SIZE);
	EXPECT_EQ(NAME,shareable.GetSharedName());
}
*/
TEST(ISharedMemoryEntity, ThrowOnGetSharedName)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedName(), std::runtime_error);
}
/*
TEST(ISharedMemoryEntity, GetSharedSize)
{
	ShareableCreator shareable;

	shareable.Create(NAME,SIZE);
	EXPECT_EQ(SIZE, shareable.GetSharedSize());
}
*/
TEST(ISharedMemoryEntity, ThrowOnGetSharedSize)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedSize(), std::runtime_error);
}
/*
TEST(ISharedMemoryEntity, GetSharedAddress)
{
	ShareableCreator shareable;

	shareable.Create(NAME,SIZE);
	EXPECT_NO_THROW(shareable.GetSharedAddress());
}
*/
TEST(ISharedMemoryEntity, ThrowOnGetSharedAddress)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedAddress(), std::runtime_error);
}
/*
int main(int argc, const char** argv)
{
	// sleep some
	boost::this_thread::sleep_for(boost::chrono::milliseconds(15000));

	std::shared_ptr<global::ISharedMemoryEntity> sable;

	// if parent
	if (argc == 1)
	{
		sable = std::make_shared<ShareableCreator>();
		sable->Create(NAME,SIZE);
	}
	else
	{
		sable = std::make_shared<ShareableAccessor>();
		sable->Open(NAME);
	}

	if (NAME != sable->GetSharedName())
		throw std::runtime_error("Shared memory segment's name is not equal to " + NAME);
	if (SIZE != sable->GetSharedSize())
		throw std::runtime_error("Shared memory segment's size is not equal to " + SIZE);
	if (!sable->GetSharedAddress())
		throw std::runtime_error("Shared memory address is null");

	if (argc == 1)
	{
		//Launch child process
		std::string executable(std::string(argv[0]) + " child ");
		if (0 != std::system(executable.c_str()))
			throw std::runtime_error("Could not launch child process while testing shared memory components.");
	}
}
*/

