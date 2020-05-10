
#include "config.h"

#include <gtest/gtest.h>

#include "Resources/IResource.h"

namespace
{
const size_t SIZE = 1;

struct Resource : resource::IResource
{
	size_t GetElementSize() const override
	{
		return SIZE;
	}

	void* Data() override
	{
		return nullptr;
	}

	const void* Data() const override
	{
		return nullptr;
	}

	void Assign(const char* buff, const size_t n) override
	{
	}
};
} // end namespace

TEST(IResource, Construct)
{
	EXPECT_NO_THROW(Resource());
}

TEST(IResource, SetColumnSize)
{
	Resource resource;
	EXPECT_NO_THROW(resource.SetColumnSize(SIZE));
	EXPECT_EQ(resource.GetColumnSize(), SIZE);
}

TEST(IResource, GetColumnSize)
{
	Resource resource;
	resource.SetColumnSize(SIZE);
	EXPECT_EQ(resource.GetColumnSize(), SIZE);
}

TEST(IResource, SetRowSize)
{
	Resource resource;
	EXPECT_NO_THROW(resource.SetRowSize(SIZE));
	EXPECT_EQ(resource.GetRowSize(), SIZE);
}

TEST(IResource, GetRowSize)
{
	Resource resource;
	resource.SetRowSize(SIZE);
	EXPECT_EQ(resource.GetRowSize(), SIZE);
}

TEST(IResource, GetElementSize)
{
	Resource resource;
	EXPECT_EQ(resource.GetElementSize(), SIZE);
}

TEST(IResource, Data)
{
	Resource resource;
	EXPECT_EQ(resource.Data(), static_cast<void*>(nullptr));
}

TEST(IResource, DataConst)
{
	const Resource resource;
	EXPECT_EQ(resource.Data(), static_cast<void*>(nullptr));
}

TEST(IResource, Assign)
{
	Resource resource;
	EXPECT_NO_THROW(resource.Assign(nullptr,SIZE));
}
