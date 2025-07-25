
#include "test_filesystem_adapters/config.h"

#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include "test_filesystem_adapters/ContainerResource.h"
#include "FilesystemAdapters/ISerializableResource.h"

using Resource = ContainerResource<int>;
using filesystem_adapters::ISerializableResource;
using LockedResource = ISerializableResource::LockedResource;

namespace
{
	const int VAL = 7;
	const int GT_BYTE_SIZE = 257;
	const std::vector<int> ARRAY_1(1, 1);
	const std::vector<int> BUFFER_2X2(4, GT_BYTE_SIZE);
	const std::vector<int> EMPTY_ARRAY_1;
} // end namespace

TEST(Resource, Construct)
{
	EXPECT_NO_THROW(Resource());
}

TEST(Resource, LValueParameterConstruct)
{
	EXPECT_NO_THROW(Resource(ARRAY_1));
	Resource resource(ARRAY_1);

	EXPECT_EQ(ARRAY_1.size(), resource.GetRowSize());
}

TEST(Resource, RValueParameterConstruct)
{
	std::vector<int> moveable = ARRAY_1;
	EXPECT_NO_THROW(Resource(std::move(moveable)));
	std::vector<int> otherMoveable = ARRAY_1;
	Resource resource(std::move(otherMoveable));

	EXPECT_EQ(ARRAY_1.size(), resource.GetRowSize());
}

TEST(Resource, Lock)
{
	Resource resource(ARRAY_1);

	LockedResource lockedResource = resource.Lock();

	EXPECT_EQ(ARRAY_1.size(), lockedResource.GetRowSize());
	EXPECT_EQ(ARRAY_1.size(), lockedResource.GetColumnSize());
	EXPECT_EQ(size_t(4), lockedResource.GetElementSize());
	EXPECT_EQ(true, lockedResource.GetDirty());

	EXPECT_EQ(*static_cast<int *>(lockedResource.Data()), ARRAY_1[0]);
	EXPECT_EQ(*static_cast<const int *>(lockedResource.Data()), ARRAY_1[0]);

	lockedResource.Assign(reinterpret_cast<const char *>(ARRAY_1.data()), ARRAY_1.size() * sizeof(int));
	EXPECT_EQ(*static_cast<int *>(lockedResource.Data()), ARRAY_1[0]);
}

TEST(Resource, GetData)
{
	Resource ir(ARRAY_1);
	EXPECT_EQ(*static_cast<int *>(ir.Data()), ARRAY_1[0]);
}

TEST(Resource, GetDataConst)
{
	const Resource ir(ARRAY_1);
	EXPECT_EQ(*static_cast<const int *>(ir.Data()), ARRAY_1[0]);
}

TEST(Resource, AssignArray)
{
	Resource ir;
	EXPECT_EQ(ARRAY_1.size(), 1);

	ir.Assign(reinterpret_cast<const char *>(ARRAY_1.data()), ARRAY_1.size() * sizeof(int));
	EXPECT_EQ(*static_cast<int *>(ir.Data()), ARRAY_1[0]);
}

TEST(Resource, AssignThrows)
{
	Resource ir;

	// resource buffer not set
	EXPECT_THROW(ir.Assign(nullptr, ARRAY_1.size() * sizeof(int)), std::runtime_error);
	// resource length is 0
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char *>(ARRAY_1.data()), 0), std::runtime_error);
	// resource length not congruent with sizeof(T)
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char *>(ARRAY_1.data()), sizeof(int) - 1), std::runtime_error);
}
