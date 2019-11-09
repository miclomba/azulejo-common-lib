
#include "config.h"

#include <iostream>
#include <vector>

#include <gtest/gtest.h>

#include "Resources/IResource.h"
#include "Resources/Resource.h"

namespace
{
const std::vector<std::vector<int>> INT_VALUES(1,std::vector<int>(1,1));
const std::vector<std::vector<int>> EMPTY_INT_VALUES;

class ContainerResource : public resource::Resource<int>
{
public:
	ContainerResource() = default;
	ContainerResource(std::vector<std::vector<int>>&& values) : Resource(std::move(values)) 
	{
		std::cout << "hello world" << std::endl;
	}
	ContainerResource(const std::vector<std::vector<int>>& values) : Resource(values)
	{
		std::cout << "hello world" << std::endl;
	}
	bool IsDirtyProtected() { return IsDirty(); }
	std::vector<int> ChecksumProtected() { return Checksum(); }
};
} // end namespace

TEST(Resource, Construct)
{
	EXPECT_NO_THROW(ContainerResource());
}

TEST(Resource, LValueParameterConstruct)
{
	EXPECT_NO_THROW(ContainerResource(INT_VALUES));
	ContainerResource resource(INT_VALUES);

	EXPECT_EQ(resource.GetColumnSize(), INT_VALUES.size());
	EXPECT_GT(INT_VALUES.size(), 0);
	EXPECT_EQ(resource.GetRowSize(), INT_VALUES[0].size());
}

TEST(Resource, RValueParameterConstruct)
{
	std::vector<std::vector<int>> moveable = INT_VALUES;
	EXPECT_NO_THROW(ContainerResource(std::move(moveable)));
	std::vector<std::vector<int>> otherMoveable = INT_VALUES;
	ContainerResource resource(std::move(otherMoveable));

	EXPECT_EQ(resource.GetColumnSize(), INT_VALUES.size());
	EXPECT_GT(INT_VALUES.size(), 0);
	EXPECT_EQ(resource.GetRowSize(), INT_VALUES[0].size());
}

TEST(Resource, ConstructThrows)
{
	const int rowSize = 1;

	std::vector<std::vector<int>> variableRowsInput;
	variableRowsInput.push_back(std::vector<int>(rowSize));
	variableRowsInput.push_back(std::vector<int>(rowSize+1));

	EXPECT_THROW(ContainerResource resource(variableRowsInput), std::invalid_argument);
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
	ir.SetColumnSize(INT_VALUES.size());
	EXPECT_GT(INT_VALUES.size(), 0);
	ir.SetRowSize(INT_VALUES[0].size());

	ir.Assign(reinterpret_cast<const char*>(INT_VALUES[0].data()), INT_VALUES.size() * INT_VALUES[0].size() * sizeof(int));
	EXPECT_EQ(ir.Data(), INT_VALUES);
}

TEST(Resource, AssignThrows)
{
	ContainerResource ir;

	// resource dimensions not set
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char*>(INT_VALUES[0].data()), INT_VALUES.size() * INT_VALUES[0].size() * sizeof(int)), std::runtime_error);

	ir.SetColumnSize(INT_VALUES.size());
	EXPECT_GT(INT_VALUES.size(), 0);
	ir.SetRowSize(INT_VALUES[0].size());

	// resource buffer not set
	EXPECT_THROW(ir.Assign(nullptr, INT_VALUES.size() * INT_VALUES[0].size() * sizeof(int)), std::runtime_error);
	// resource length is 0 
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char*>(INT_VALUES[0].data()), 0), std::runtime_error);
	// resource length not congruent with sizeof(T) 
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char*>(INT_VALUES[0].data()), sizeof(int)-1), std::runtime_error);
}

TEST(Resource, SetColumnSize)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_EQ(ir.GetColumnSize(), 0);
	EXPECT_NO_THROW(ir.SetColumnSize(size));
	EXPECT_EQ(ir.GetColumnSize(), size);
}

TEST(Resource, SetColumnSizeThrowsWhenChangingDimension)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_NO_THROW(ir.SetColumnSize(size));
	EXPECT_NO_THROW(ir.SetColumnSize(size));
	EXPECT_THROW(ir.SetColumnSize(size + 1), std::runtime_error);
}

TEST(Resource, SetRowSize)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_EQ(ir.GetRowSize(), 0);
	EXPECT_NO_THROW(ir.SetRowSize(size));
	EXPECT_EQ(ir.GetRowSize(), size);
}

TEST(Resource, SetRowSizeThrowsWhenChangingDimension)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_NO_THROW(ir.SetRowSize(size));
	EXPECT_NO_THROW(ir.SetRowSize(size));
	EXPECT_THROW(ir.SetRowSize(size + 1), std::runtime_error);
}

TEST(Resource, GetColumnSize)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_EQ(ir.GetColumnSize(), 0);
	EXPECT_NO_THROW(ir.SetColumnSize(size));
	EXPECT_EQ(ir.GetColumnSize(), size);
}

TEST(Resource, GetRowSize)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_EQ(ir.GetRowSize(), 0);
	EXPECT_NO_THROW(ir.SetRowSize(size));
	EXPECT_EQ(ir.GetRowSize(), size);
}
