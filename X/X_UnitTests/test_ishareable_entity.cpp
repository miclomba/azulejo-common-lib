#include "gtest/gtest.h"

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

#include "X/IShareableEntity.h"

namespace
{
const std::string NAME = "shmem";
const size_t SIZE = 1024;

class ShareableCreator : public global::IShareableEntity
{
public:
	ShareableCreator() {};

	void Share(const std::string& name) override { Create(name,SIZE); };
};

class ShareableAccessor : public global::IShareableEntity
{
public:
	ShareableAccessor() {};

	void Share(const std::string& name) override { Open(name); };
};
}
/*
TEST(IShareableEntity, OpenOrCreate)
{
	ShareableCreator shareable;

	EXPECT_NO_THROW(shareable.Share(NAME));
}

TEST(IShareableEntity, ThrowOnOpenOrCreate)
{
	ShareableCreator shareable;

	EXPECT_NO_THROW(shareable.Share(NAME));
	EXPECT_THROW(shareable.Share(NAME), std::runtime_error);
}

TEST(IShareableEntity, GetSharedName)
{
	ShareableCreator shareable;

	shareable.Share(NAME);
	EXPECT_EQ(NAME,shareable.GetSharedName());
}
*/
TEST(IShareableEntity, ThrowOnGetSharedName)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedName(), std::runtime_error);
}
/*
TEST(IShareableEntity, GetSharedSize)
{
	ShareableCreator shareable;

	shareable.Share(NAME);
	EXPECT_EQ(SIZE, shareable.GetSharedSize());
}
*/
TEST(IShareableEntity, ThrowOnGetSharedSize)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedSize(), std::runtime_error);
}
/*
TEST(IShareableEntity, GetSharedAddress)
{
	ShareableCreator shareable;

	shareable.Share(NAME);
	EXPECT_NO_THROW(shareable.GetSharedAddress());
}
*/
TEST(IShareableEntity, ThrowOnGetSharedAddress)
{
	ShareableCreator shareable;

	EXPECT_THROW(shareable.GetSharedAddress(), std::runtime_error);
}
/*
int main(int argc, const char** argv)
{
	// sleep some
	boost::this_thread::sleep_for(boost::chrono::milliseconds(15000));

	std::shared_ptr<global::IShareableEntity> sable;

	// if parent
	if (argc == 1)
		sable = std::make_shared<ShareableCreator>();
	else
		sable = std::make_shared<ShareableAccessor>();
		
	sable->Share(NAME);

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

