
#include "config.h"

#include <array>
#include <vector>

#include <gtest/gtest.h>

#include "X/Resource.h"

namespace
{
const std::vector<int> INT_VALUES(1,1);

class IntResource : public dataresource::Resource<std::vector<int>>
{
public:
	IntResource() {}
	IntResource(const std::vector<int>& values) 
	{
		EXPECT_NO_THROW(SetData(values));
		EXPECT_EQ(GetData(), values);
	}
	std::vector<int> GetData() const { return Data(); }
	void SetData(const std::vector<int>& values) { Data() = values; }
	int ChecksumProtected() { return Checksum(); }
};
}

TEST(Resource, MoveConstruct)
{
	IntResource ir(INT_VALUES);

	// move
	IntResource irMoved(std::move(ir));

	EXPECT_EQ(irMoved.GetData(), INT_VALUES);
}

TEST(Resource, MoveAssign)
{
	IntResource ir(INT_VALUES);
	IntResource irMoved;

	// move assign
	EXPECT_NO_THROW(irMoved = std::move(ir));

	EXPECT_EQ(irMoved.GetData(), INT_VALUES);
}

TEST(Resource, CopyConstruct)
{
	IntResource ir(INT_VALUES);

	// copy
	IntResource irCopied(ir);
	EXPECT_EQ(irCopied.GetData(), INT_VALUES);
}

TEST(Resource, CopyAssign)
{
	IntResource ir(INT_VALUES);
	IntResource irCopied;

	// copy assign
	EXPECT_NO_THROW(irCopied = ir);

	EXPECT_EQ(irCopied.GetData(), INT_VALUES);
}

TEST(Resource, SetData)
{
	IntResource ir;
	EXPECT_NO_THROW(ir.SetData(INT_VALUES));
	EXPECT_EQ(ir.GetData(), INT_VALUES);
}

TEST(Resource, GetData)
{
	IntResource ir;
	EXPECT_NO_THROW(ir.SetData(INT_VALUES));
	EXPECT_EQ(ir.GetData(), INT_VALUES);
}

TEST(Resource, IsDirty)
{
	IntResource ir;
	EXPECT_TRUE(ir.IsDirty());
	EXPECT_FALSE(ir.IsDirty());

	ir.SetData(INT_VALUES);
	EXPECT_TRUE(ir.IsDirty());
}

TEST(Resource, Checksum)
{
	IntResource ir;

	int checksum = ir.ChecksumProtected();
	EXPECT_EQ(ir.ChecksumProtected(), checksum);

	ir.SetData(INT_VALUES);
	EXPECT_NE(ir.ChecksumProtected(), checksum);
}
