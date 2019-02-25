
#include "config.h"

#include <array>
#include <vector>

#include <gtest/gtest.h>

#include "Resources/IResource.h"
#include "Resources/Resource.h"

typedef int Int;

namespace
{
const std::vector<int> INT_VALUES(1,1);
const std::vector<int> EMPTY_INT_VALUES;

class ContainerResource : public resource::Resource<std::vector<Int>>
{
public:
	ContainerResource() = default;
	ContainerResource(std::vector<Int>&& values) : Resource(std::move(values)) {}
	ContainerResource(const std::vector<Int>& values) : Resource(values) {}
	bool IsDirtyProtected() { return IsDirty(); }
	int ChecksumProtected() { return Checksum(); }
};
}

TEST(Resource, Construct)
{
	EXPECT_NO_THROW(ContainerResource());
}

TEST(Resource, MoveConstruct)
{
	ContainerResource ir(INT_VALUES);

	// move
	ContainerResource irMoved(std::move(ir));

	EXPECT_EQ(irMoved.Data(), INT_VALUES);
}

TEST(Resource, MoveAssign)
{
	ContainerResource ir(INT_VALUES);
	ContainerResource irMoved(EMPTY_INT_VALUES);

	// move assign
	EXPECT_NO_THROW(irMoved = std::move(ir));

	EXPECT_EQ(irMoved.Data(), INT_VALUES);
}

TEST(Resource, CopyConstruct)
{
	ContainerResource ir(INT_VALUES);

	// copy
	ContainerResource irCopied(ir);
	EXPECT_EQ(irCopied.Data(), INT_VALUES);
}

TEST(Resource, CopyAssign)
{
	ContainerResource ir(INT_VALUES);
	ContainerResource irCopied(EMPTY_INT_VALUES);

	// copy assign
	EXPECT_NO_THROW(irCopied = ir);

	EXPECT_EQ(irCopied.Data(), INT_VALUES);
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

	int checksum = ir.ChecksumProtected();
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
