
#include "config.h"

#include <array>
#include <vector>

#include <gtest/gtest.h>

#include "X/IResource.h"

namespace
{
const std::vector<int> INT_VALUES(1,1);
const std::vector<int> EMPTY_INT_VALUES;

class IntResource : public dataresource::IResource<std::vector<int>>
{
public:
	IntResource(std::vector<int>&& values) : IResource(std::move(values)) {}
	IntResource(const std::vector<int>& values) : IResource(values) {}
	std::vector<int> GetData() const { return Data(); }
	void SetData(const std::vector<int>& values) { Data() = values; }
	bool IsDirtyProtected() { return IsDirty(); }
	int ChecksumProtected() { return Checksum(); }

protected:
	void SaveImpl(const std::string& path) const {}
	void LoadImpl(const std::string& path) {};
};
}

TEST(IResource, MoveConstruct)
{
	IntResource ir(INT_VALUES);

	// move
	IntResource irMoved(std::move(ir));

	EXPECT_EQ(irMoved.GetData(), INT_VALUES);
}

TEST(IResource, MoveAssign)
{
	IntResource ir(INT_VALUES);
	IntResource irMoved(EMPTY_INT_VALUES);

	// move assign
	EXPECT_NO_THROW(irMoved = std::move(ir));

	EXPECT_EQ(irMoved.GetData(), INT_VALUES);
}

TEST(IResource, CopyConstruct)
{
	IntResource ir(INT_VALUES);

	// copy
	IntResource irCopied(ir);
	EXPECT_EQ(irCopied.GetData(), INT_VALUES);
}

TEST(IResource, CopyAssign)
{
	IntResource ir(INT_VALUES);
	IntResource irCopied(EMPTY_INT_VALUES);

	// copy assign
	EXPECT_NO_THROW(irCopied = ir);

	EXPECT_EQ(irCopied.GetData(), INT_VALUES);
}

TEST(IResource, SetData)
{
	IntResource ir(EMPTY_INT_VALUES);
	EXPECT_NO_THROW(ir.SetData(INT_VALUES));
	EXPECT_EQ(ir.GetData(), INT_VALUES);
}

TEST(IResource, GetData)
{
	IntResource ir(EMPTY_INT_VALUES);
	EXPECT_NO_THROW(ir.SetData(INT_VALUES));
	EXPECT_EQ(ir.GetData(), INT_VALUES);
}

TEST(IResource, IsDirty)
{
	IntResource ir(EMPTY_INT_VALUES);
	EXPECT_TRUE(ir.IsDirtyProtected());
	EXPECT_FALSE(ir.IsDirtyProtected());

	ir.SetData(INT_VALUES);
	EXPECT_TRUE(ir.IsDirtyProtected());
}

TEST(IResource, Checksum)
{
	IntResource ir(EMPTY_INT_VALUES);

	int checksum = ir.ChecksumProtected();
	EXPECT_EQ(ir.ChecksumProtected(), checksum);

	ir.SetData(INT_VALUES);
	EXPECT_NE(ir.ChecksumProtected(), checksum);
}

