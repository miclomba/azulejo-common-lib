#include "gtest/gtest.h"

#include "X/Resource.h"

namespace
{
const int INT_VALUE = 7;

class IntResource : public global::Resource<int>
{
public:
	IntResource() {}
	IntResource(const int value) 
	{
		EXPECT_NO_THROW(SetData(value));
		EXPECT_EQ(GetData(), value);
	}
	int GetData() const { return Data(); }
	void SetData(int value) { Data() = value; }
};
}

TEST(Resource, MoveConstruct)
{
	IntResource ir(INT_VALUE);

	// move
	IntResource irMoved(std::move(ir));

	EXPECT_EQ(irMoved.GetData(), INT_VALUE);
}

TEST(Resource, MoveAssign)
{
	IntResource ir(INT_VALUE);
	IntResource irMoved;

	// move assign
	EXPECT_NO_THROW(irMoved = std::move(ir));

	EXPECT_EQ(irMoved.GetData(), INT_VALUE);
}

TEST(Resource, CopyConstruct)
{
	IntResource ir(INT_VALUE);

	// copy
	IntResource irCopied(ir);
	EXPECT_EQ(irCopied.GetData(), INT_VALUE);
}

TEST(Resource, CopyAssign)
{
	IntResource ir(INT_VALUE);
	IntResource irCopied;

	// copy assign
	EXPECT_NO_THROW(irCopied = ir);

	EXPECT_EQ(irCopied.GetData(), INT_VALUE);
}

TEST(Resource, SetData)
{
	IntResource ir;
	EXPECT_NO_THROW(ir.SetData(INT_VALUE));
	EXPECT_EQ(ir.GetData(), INT_VALUE);
}

TEST(Resource, GetData)
{
	IntResource ir;
	EXPECT_NO_THROW(ir.SetData(INT_VALUE));
	EXPECT_EQ(ir.GetData(), INT_VALUE);
}