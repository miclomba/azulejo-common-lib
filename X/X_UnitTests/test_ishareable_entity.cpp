#include "gtest/gtest.h"

#include "X/IShareableEntity.h"

namespace
{
const std::string NAME = "shmem";
const size_t SIZE = 1024;

class Shareable : public global::IShareableEntity
{
public:
	Shareable() {};

	void Share(const std::string& name) override { OpenOrCreate(name,SIZE); };
	void PublicOpenOrCreate(const std::string& name, const size_t size) { OpenOrCreate(name,size); };
};
}

TEST(IShareableEntity, OpenOrCreate)
{
	Shareable shareable;

	EXPECT_NO_THROW(shareable.Share(NAME));
}

TEST(IShareableEntity, ThrowOnOpenOrCreate)
{
	Shareable shareable;

	EXPECT_NO_THROW(shareable.Share(NAME));
	EXPECT_THROW(shareable.Share(NAME), std::runtime_error);
}

TEST(IShareableEntity, GetSharedName)
{
	Shareable shareable;

	shareable.Share(NAME);
	EXPECT_EQ(NAME,shareable.GetSharedName());
}

TEST(IShareableEntity, ThrowOnGetSharedName)
{
	Shareable shareable;

	EXPECT_THROW(shareable.GetSharedName(), std::runtime_error);
}

TEST(IShareableEntity, GetSharedSize)
{
	Shareable shareable;

	shareable.Share(NAME);
	EXPECT_EQ(SIZE, shareable.GetSharedSize());
}

TEST(IShareableEntity, ThrowOnGetSharedSize)
{
	Shareable shareable;

	EXPECT_THROW(shareable.GetSharedSize(), std::runtime_error);
}

TEST(IShareableEntity, GetSharedAddress)
{
	Shareable shareable;

	shareable.Share(NAME);
	EXPECT_NO_THROW(shareable.GetSharedAddress());
}

TEST(IShareableEntity, ThrowOnGetSharedAddress)
{
	Shareable shareable;

	EXPECT_THROW(shareable.GetSharedAddress(), std::runtime_error);
}

