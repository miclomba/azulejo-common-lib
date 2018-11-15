#include "gtest/gtest.h"

#include "X/Entity.h"

namespace
{
const int INT_VALUE = 7;

class IntEntity : public global::Entity<int>
{
public:
	IntEntity() {}
	IntEntity(const int value) 
	{
		EXPECT_NO_THROW(SetResource(value));
		EXPECT_EQ(GetResource(), value);
	}
	int GetResource() const { return Data(); }
	void SetResource(int value) { Data() = value; }
};
}

TEST(Entity, MoveConstruct)
{
	IntEntity ie(INT_VALUE);

	// move
	IntEntity ieMoved(std::move(ie));

	EXPECT_EQ(ieMoved.GetResource(), INT_VALUE);
}

TEST(Entity, MoveAssign)
{
	IntEntity ie(INT_VALUE);
	IntEntity ieMoved;

	// move assign
	EXPECT_NO_THROW(ieMoved = std::move(ie));

	EXPECT_EQ(ieMoved.GetResource(), INT_VALUE);
}

TEST(Entity, CopyConstruct)
{
	IntEntity ie(INT_VALUE);

	// copy
	IntEntity ieCopied(ie);
	EXPECT_EQ(ieCopied.GetResource(), INT_VALUE);
}

TEST(Entity, CopyAssign)
{
	IntEntity ie(INT_VALUE);
	IntEntity ieCopied;

	// copy assign
	EXPECT_NO_THROW(ieCopied = ie);

	EXPECT_EQ(ieCopied.GetResource(), INT_VALUE);
}

TEST(Entity, SetResource)
{
	IntEntity ie;
	EXPECT_NO_THROW(ie.SetResource(INT_VALUE));
	EXPECT_EQ(ie.GetResource(), INT_VALUE);
}

TEST(Entity, GetResource)
{
	IntEntity ie;
	EXPECT_NO_THROW(ie.SetResource(INT_VALUE));
	EXPECT_EQ(ie.GetResource(), INT_VALUE);
}