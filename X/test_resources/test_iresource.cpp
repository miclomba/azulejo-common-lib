
#include "config.h"

#include <array>
#include <vector>

#include <gtest/gtest.h>

#include "Resources/IResource.h"

namespace
{
const std::vector<int> INT_VALUES(1,1);
const std::vector<int> EMPTY_INT_VALUES;

class ContainerResource : public resource::IResource<std::vector<int>>
{
public:
	ContainerResource(std::vector<int>&& values) : IResource(std::move(values)) {}
	ContainerResource(const std::vector<int>& values) : IResource(values) {}
	std::vector<int> GetData() const { return Data(); }
	void SetData(const std::vector<int>& values) { Data() = values; }
	bool IsDirtyProtected() { return IsDirty(); }
	int ChecksumProtected() { return Checksum(); }
};
}

TEST(IResource, MoveConstruct)
{
	ContainerResource ir(INT_VALUES);

	// move
	ContainerResource irMoved(std::move(ir));

	EXPECT_EQ(irMoved.GetData(), INT_VALUES);
}

TEST(IResource, MoveAssign)
{
	ContainerResource ir(INT_VALUES);
	ContainerResource irMoved(EMPTY_INT_VALUES);

	// move assign
	EXPECT_NO_THROW(irMoved = std::move(ir));

	EXPECT_EQ(irMoved.GetData(), INT_VALUES);
}

TEST(IResource, CopyConstruct)
{
	ContainerResource ir(INT_VALUES);

	// copy
	ContainerResource irCopied(ir);
	EXPECT_EQ(irCopied.GetData(), INT_VALUES);
}

TEST(IResource, CopyAssign)
{
	ContainerResource ir(INT_VALUES);
	ContainerResource irCopied(EMPTY_INT_VALUES);

	// copy assign
	EXPECT_NO_THROW(irCopied = ir);

	EXPECT_EQ(irCopied.GetData(), INT_VALUES);
}

TEST(IResource, SetData)
{
	ContainerResource ir(EMPTY_INT_VALUES);
	EXPECT_NO_THROW(ir.SetData(INT_VALUES));
	EXPECT_EQ(ir.GetData(), INT_VALUES);
}

TEST(IResource, GetData)
{
	ContainerResource ir(EMPTY_INT_VALUES);
	EXPECT_NO_THROW(ir.SetData(INT_VALUES));
	EXPECT_EQ(ir.GetData(), INT_VALUES);
}

TEST(IResource, IsDirty)
{
	ContainerResource ir(EMPTY_INT_VALUES);
	EXPECT_TRUE(ir.IsDirtyProtected());
	EXPECT_FALSE(ir.IsDirtyProtected());

	ir.SetData(INT_VALUES);
	EXPECT_TRUE(ir.IsDirtyProtected());
}

TEST(IResource, Checksum)
{
	ContainerResource ir(EMPTY_INT_VALUES);

	int checksum = ir.ChecksumProtected();
	EXPECT_EQ(ir.ChecksumProtected(), checksum);

	ir.SetData(INT_VALUES);
	EXPECT_NE(ir.ChecksumProtected(), checksum);
}
/*
TEST(IResource, Save)
{
	ContainerResource ir(EMPTY_INT_VALUES);

	EXPECT_THROW(ir.Save(""), std::runtime_error);
	EXPECT_NO_THROW(ir.Save(""));
}

TEST(IResource, Load)
{
	ContainerResource ir(EMPTY_INT_VALUES);

	EXPECT_NO_THROW(ir.Load(""));
}
*/
