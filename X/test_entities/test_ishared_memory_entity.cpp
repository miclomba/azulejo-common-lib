//#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>

#include "config.h"

#include <gtest/gtest.h>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

#include "Entities/ISharedMemoryEntity.h"

using entity::ISharedMemoryEntity;

namespace
{
const std::string NAME = "shmem";
const std::string OTHER_NAME = "other_shmem";
const size_t SIZE = 1024;

struct ShareableCreator : public ISharedMemoryEntity {};
struct ShareableAccessor : public ISharedMemoryEntity {};
} // end namespace anonymous

TEST(ISharedMemoryEntity, Construct)
{
	EXPECT_NO_THROW(ShareableCreator shareable());
}

TEST(ISharedMemoryEntity, CopyConstruct)
{
	ShareableCreator shareable;
	EXPECT_NO_THROW(ShareableCreator copy(shareable));
}

TEST(ISharedMemoryEntity, CopyAssign)
{
	ShareableCreator shareable;
	ShareableCreator copy;
	EXPECT_NO_THROW(copy = shareable);
}

TEST(ISharedMemoryEntity, MoveConstruct)
{
	ShareableCreator shareable;
	EXPECT_NO_THROW(ShareableCreator copy(std::move(shareable)));
}

TEST(ISharedMemoryEntity, MoveAssign)
{
	ShareableCreator shareable;
	ShareableCreator copy;
	EXPECT_NO_THROW(copy = std::move(shareable));
}

TEST(ISharedMemoryEntity, Create)
{
	ShareableCreator shareable;

	EXPECT_NO_THROW(shareable.Create(NAME,SIZE));
}

TEST(ISharedMemoryEntity, CreateThrowsWhenSharedMemoryAlreadyExists)
{
	ShareableCreator shareable;
	ShareableCreator otherShareable;

	EXPECT_NO_THROW(shareable.Create(NAME, SIZE));
	EXPECT_THROW(otherShareable.Create(NAME, SIZE), std::runtime_error);
}

TEST(ISharedMemoryEntity, CreateWhenAlreadyCreatedThrows)
{
	ShareableCreator shareable;

	EXPECT_NO_THROW(shareable.Create(NAME, SIZE));
	EXPECT_THROW(shareable.Create(OTHER_NAME, SIZE), std::runtime_error);
}

TEST(ISharedMemoryEntity, CreateUsingEmptyNameThrows)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.Create("", SIZE), std::runtime_error);
}

TEST(ISharedMemoryEntity, CreateUsingInvalidSizeThrows)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.Create(NAME, 0), std::runtime_error);
}

TEST(ISharedMemoryEntity, Open)
{
	ShareableCreator shareable;
	ShareableCreator otherShareable;

	EXPECT_NO_THROW(shareable.Create(NAME,SIZE));
	EXPECT_NO_THROW(otherShareable.Open(NAME));
}

TEST(ISharedMemoryEntity, OpenThrows)
{
	ShareableCreator shareable;
	ShareableCreator otherShareable;

	EXPECT_NO_THROW(shareable.Create(NAME, SIZE));
	EXPECT_THROW(otherShareable.Open(OTHER_NAME), std::runtime_error);
}

TEST(ISharedMemoryEntity, GetSharedName)
{
	ShareableCreator shareable;

	shareable.Create(NAME,SIZE);
	EXPECT_EQ(NAME,shareable.GetSharedName());
}

TEST(ISharedMemoryEntity, ThrowOnGetSharedName)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedName(), std::runtime_error);
}

TEST(ISharedMemoryEntity, GetSharedSize)
{
	ShareableCreator shareable;

	shareable.Create(NAME,SIZE);
	EXPECT_EQ(SIZE, shareable.GetSharedSize());
}

TEST(ISharedMemoryEntity, ThrowOnGetSharedSize)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedSize(), std::runtime_error);
}

TEST(ISharedMemoryEntity, GetSharedAddress)
{
	ShareableCreator shareable;

	shareable.Create(NAME,SIZE);
	EXPECT_NO_THROW(shareable.GetSharedAddress());
}

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

	std::shared_ptr<entity::ISharedMemoryEntity> sable;

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

