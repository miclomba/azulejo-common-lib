
#include "config.h"

#include <vector>

#include <gtest/gtest.h>

#include "Resources/IResource.h"
#include "Resources/Resource.h"

namespace
{
const std::vector<int> INT_VALUES(1,1);
const std::vector<int> EMPTY_INT_VALUES;

class ContainerResource : public resource::Resource<int>
{
public:
	ContainerResource() = default;
	ContainerResource(std::vector<int>&& values) : Resource(std::move(values)) {}
	ContainerResource(const std::vector<int>& values) : Resource(values) {}
	bool IsDirtyProtected() { return IsDirty(); }
	std::vector<int> ChecksumProtected() { return Checksum(); }
};
} // end namespace

TEST(Resource, Construct)
{
	EXPECT_NO_THROW(ContainerResource());
}

TEST(Resource, MoveConstruct)
{
	ContainerResource source(INT_VALUES);
	std::vector<int> sourceChecksum = source.ChecksumProtected();

	// move
	ContainerResource target(std::move(source));

	EXPECT_EQ(target.Data(), INT_VALUES);
	EXPECT_EQ(target.ChecksumProtected(), sourceChecksum);
}

TEST(Resource, MoveAssign)
{
	ContainerResource source(INT_VALUES);
	std::vector<int> sourceChecksum = source.ChecksumProtected();
	ContainerResource target(EMPTY_INT_VALUES);

	// move assign
	EXPECT_NO_THROW(target = std::move(source));

	EXPECT_EQ(target.Data(), INT_VALUES);
	EXPECT_EQ(target.ChecksumProtected(), sourceChecksum);
}

TEST(Resource, CopyConstruct)
{
	ContainerResource source(INT_VALUES);

	// copy
	ContainerResource target(source);
	EXPECT_EQ(target.Data(), INT_VALUES);
	EXPECT_EQ(target.ChecksumProtected(), source.ChecksumProtected());
}

TEST(Resource, CopyAssign)
{
	ContainerResource source(INT_VALUES);
	ContainerResource target(EMPTY_INT_VALUES);

	// copy assign
	EXPECT_NO_THROW(target = source);

	EXPECT_EQ(target.Data(), INT_VALUES);
	EXPECT_EQ(target.ChecksumProtected(), source.ChecksumProtected());
}

TEST(Resource, SetData)
{
	ContainerResource ir(EMPTY_INT_VALUES);
	EXPECT_NO_THROW(ir.Data() = INT_VALUES);
	EXPECT_EQ(ir.Data(), INT_VALUES);
}

TEST(Resource, GetData)
{
	ContainerResource ir(EMPTY_INT_VALUES);
	EXPECT_NO_THROW(ir.Data() = INT_VALUES);
	EXPECT_EQ(ir.Data(), INT_VALUES);
}

TEST(Resource, IsDirty)
{
	ContainerResource ir(EMPTY_INT_VALUES);
	EXPECT_TRUE(ir.IsDirtyProtected());
	EXPECT_FALSE(ir.IsDirtyProtected());

	ir.Data() = INT_VALUES;
	EXPECT_TRUE(ir.IsDirtyProtected());
}

TEST(Resource, Checksum)
{
	ContainerResource ir(EMPTY_INT_VALUES);

	std::vector<int> checksum = ir.ChecksumProtected();
	EXPECT_EQ(ir.ChecksumProtected(), checksum);

	ir.Data() = INT_VALUES;
	EXPECT_NE(ir.ChecksumProtected(), checksum);
}

TEST(Resource, Assign)
{
	ContainerResource ir;

	ir.Assign(reinterpret_cast<const char*>(INT_VALUES.data()), INT_VALUES.size() * sizeof(int));
	EXPECT_EQ(ir.Data(), INT_VALUES);
}

TEST(Resource, AssignThrows)
{
	ContainerResource ir;

	EXPECT_THROW(ir.Assign(nullptr, INT_VALUES.size() * sizeof(int)), std::runtime_error);
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char*>(INT_VALUES.data()), 0), std::runtime_error);
}
