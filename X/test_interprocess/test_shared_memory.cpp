//#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>

#include "config.h"

#include <gtest/gtest.h>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

#include "Interprocess/ISharedMemory.h"

using interprocess::ISharedMemory;

namespace
{
const std::string NAME = "shmem";
const std::string OTHER_NAME = "other_shmem";
const size_t SIZE = 1024;

struct ShareableCreator : public ISharedMemory {};
} // end namespace anonymous

TEST(ISharedMemory, Construct)
{
	EXPECT_NO_THROW(ShareableCreator shareable());
}

TEST(ISharedMemory, Create)
{
	ShareableCreator shareable;

	EXPECT_NO_THROW(shareable.Create(NAME,SIZE));
}

TEST(ISharedMemory, CreateThrowsWhenSharedMemoryAlreadyExists)
{
	ShareableCreator shareable;
	ShareableCreator otherShareable;

	EXPECT_NO_THROW(shareable.Create(NAME, SIZE));
	EXPECT_THROW(otherShareable.Create(NAME, SIZE), std::runtime_error);
}

TEST(ISharedMemory, CreateWhenAlreadyCreatedThrows)
{
	ShareableCreator shareable;

	EXPECT_NO_THROW(shareable.Create(NAME, SIZE));
	EXPECT_THROW(shareable.Create(OTHER_NAME, SIZE), std::runtime_error);
}

TEST(ISharedMemory, CreateUsingEmptyNameThrows)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.Create("", SIZE), std::runtime_error);
}

TEST(ISharedMemory, CreateUsingInvalidSizeThrows)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.Create(NAME, 0), std::runtime_error);
}

TEST(ISharedMemory, Open)
{
	ShareableCreator shareable;
	ShareableCreator otherShareable;

	EXPECT_NO_THROW(shareable.Create(NAME,SIZE));
	EXPECT_NO_THROW(otherShareable.Open(NAME));
}

TEST(ISharedMemory, OpenThrows)
{
	ShareableCreator shareable;
	ShareableCreator otherShareable;

	EXPECT_NO_THROW(shareable.Create(NAME, SIZE));
	EXPECT_THROW(otherShareable.Open(OTHER_NAME), std::runtime_error);
}

TEST(ISharedMemory, Destroy)
{
	ShareableCreator shareable;
	ShareableCreator otherShareable;

	EXPECT_NO_THROW(shareable.Create(NAME, SIZE));
	EXPECT_TRUE(shareable.IsSharedMemoryOwner());
	EXPECT_NO_THROW(otherShareable.Open(NAME));
	EXPECT_FALSE(otherShareable.IsSharedMemoryOwner());

	EXPECT_EQ(shareable.GetSharedAddress(), otherShareable.GetSharedAddress());
}

TEST(ISharedMemory, GetSharedName)
{
	ShareableCreator shareable;

	shareable.Create(NAME,SIZE);
	EXPECT_EQ(NAME,shareable.GetSharedName());
}

TEST(ISharedMemory, ThrowOnGetSharedName)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedName(), std::runtime_error);
}

TEST(ISharedMemory, GetSharedSize)
{
	ShareableCreator shareable;

	shareable.Create(NAME,SIZE);
	EXPECT_EQ(SIZE, shareable.GetSharedSize());
}

TEST(ISharedMemory, ThrowOnGetSharedSize)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedSize(), std::runtime_error);
}

TEST(ISharedMemory, GetSharedAddress)
{
	ShareableCreator shareable;

	shareable.Create(NAME,SIZE);
	EXPECT_NO_THROW(shareable.GetSharedAddress());
}

TEST(ISharedMemory, ThrowOnGetSharedAddress)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedAddress(), std::runtime_error);
}

TEST(ISharedMemory, IsSharedMemoryOwner)
{
	ShareableCreator shareable;
	EXPECT_FALSE(shareable.IsSharedMemoryOwner());

	shareable.Create(NAME, SIZE);
	EXPECT_TRUE(shareable.IsSharedMemoryOwner());
}

/*
int main(int argc, const char** argv)
{
	// sleep some
	boost::this_thread::sleep_for(boost::chrono::milliseconds(15000));

	std::shared_ptr<interprocess::ISharedMemory> sable;

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

