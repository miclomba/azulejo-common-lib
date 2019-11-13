
#include "config.h"

#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include "Resources/IResource.h"
#include "Resources/Resource.h"

namespace
{
const int VAL = 7;
const int GT_BYTE_SIZE = 257;
const std::vector<int> ARRAY_1(1,1);
const std::vector<int> BUFFER_2X2(4,GT_BYTE_SIZE);
const std::vector<int> EMPTY_ARRAY_1;

class ContainerResource : public resource::Resource<int>
{
public:
	ContainerResource() = default;
	ContainerResource(std::vector<int>&& values) : Resource(std::move(values)) {}
	ContainerResource(const std::vector<int>& values) : Resource(values) {}
	bool IsDirtyProtected() { return IsDirty(); }
	int ChecksumProtected() { return Checksum(); }
};
} // end namespace

TEST(Resource, Construct)
{
	EXPECT_NO_THROW(ContainerResource());
}

TEST(Resource, LValueParameterConstruct)
{
	EXPECT_NO_THROW(ContainerResource(ARRAY_1));
	ContainerResource resource(ARRAY_1);

	EXPECT_EQ(ARRAY_1.size(), resource.GetRowSize());
}

TEST(Resource, RValueParameterConstruct)
{
	std::vector<int> moveable = ARRAY_1;
	EXPECT_NO_THROW(ContainerResource(std::move(moveable)));
	std::vector<int> otherMoveable = ARRAY_1;
	ContainerResource resource(std::move(otherMoveable));

	EXPECT_EQ(ARRAY_1.size(), resource.GetRowSize());
}

TEST(Resource, MoveConstruct)
{
	ContainerResource source(ARRAY_1);
	int sourceChecksum = source.ChecksumProtected();

	// move
	ContainerResource target(std::move(source));

	EXPECT_EQ(*target.Data(), ARRAY_1[0]);
	EXPECT_EQ(target.ChecksumProtected(), sourceChecksum);
}

TEST(Resource, MoveAssign)
{
	ContainerResource source(ARRAY_1);
	int sourceChecksum = source.ChecksumProtected();
	ContainerResource target(EMPTY_ARRAY_1);

	// move assign
	EXPECT_NO_THROW(target = std::move(source));

	EXPECT_EQ(*target.Data(), ARRAY_1[0]);
	EXPECT_EQ(target.ChecksumProtected(), sourceChecksum);
}

TEST(Resource, CopyConstruct)
{
	ContainerResource source(ARRAY_1);

	// copy
	ContainerResource target(source);
	EXPECT_EQ(*target.Data(), ARRAY_1[0]);
	EXPECT_EQ(target.ChecksumProtected(), source.ChecksumProtected());
}

TEST(Resource, CopyAssign)
{
	ContainerResource source(ARRAY_1);
	ContainerResource target(EMPTY_ARRAY_1);

	// copy assign
	EXPECT_NO_THROW(target = source);

	EXPECT_EQ(*target.Data(), ARRAY_1[0]);
	EXPECT_EQ(target.ChecksumProtected(), source.ChecksumProtected());
}

TEST(Resource, GetData)
{
	ContainerResource ir(ARRAY_1);
	EXPECT_EQ(*ir.Data(), ARRAY_1[0]);
}

TEST(Resource, GetDataConst)
{
	const ContainerResource ir(ARRAY_1);
	EXPECT_EQ(*ir.Data(), ARRAY_1[0]);
}

TEST(Resource, IsDirty)
{
	std::vector<int> values = ARRAY_1;
	ContainerResource ir(ARRAY_1);

	EXPECT_TRUE(ir.IsDirtyProtected());
	EXPECT_FALSE(ir.IsDirtyProtected());

	*ir.Data() = VAL;
	EXPECT_TRUE(ir.IsDirtyProtected());
}

TEST(Resource, Checksum)
{
	ContainerResource ir(ARRAY_1);

	int checksum = ir.ChecksumProtected();
	EXPECT_EQ(ir.ChecksumProtected(), checksum);

	*ir.Data() = VAL;
	EXPECT_NE(ir.ChecksumProtected(), checksum);
}

TEST(Resource, AssignArray)
{
	ContainerResource ir;
	EXPECT_EQ(ARRAY_1.size(), 1);

	ir.Assign(reinterpret_cast<const char*>(ARRAY_1.data()), ARRAY_1.size() * sizeof(int));
	EXPECT_EQ(*ir.Data(), ARRAY_1[0]);
}

TEST(Resource, AssignThrows)
{
	ContainerResource ir;

	// resource buffer not set
	EXPECT_THROW(ir.Assign(nullptr, ARRAY_1.size() * sizeof(int)), std::runtime_error);
	// resource length is 0 
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char*>(ARRAY_1.data()), 0), std::runtime_error);
	// resource length not congruent with sizeof(T) 
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char*>(ARRAY_1.data()), sizeof(int)-1), std::runtime_error);
}
