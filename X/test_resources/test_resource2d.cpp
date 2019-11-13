
#include "config.h"

#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include "Resources/IResource.h"
#include "Resources/Resource.h"
#include "Resources/Resource2D.h"

namespace
{
const int VAL = 7;
const int GT_BYTE_SIZE = 257;
const std::vector<std::vector<int>> MATRIX_1X1(1,std::vector<int>(1,1));
const std::vector<int> BUFFER_2X2(4,GT_BYTE_SIZE);
const std::vector<std::vector<int>> EMPTY_MATRIX_1X1;

class ContainerResource : public resource::Resource2D<int>
{
public:
	ContainerResource() = default;
	ContainerResource(std::vector<std::vector<int>>&& values) : Resource2D(std::move(values)) {}
	ContainerResource(const std::vector<std::vector<int>>& values) : Resource2D(values) {}
	int ChecksumProtected() { return Checksum(); }
};
} // end namespace

TEST(Resource2D, Construct)
{
	EXPECT_NO_THROW(ContainerResource());
}

TEST(Resource2D, LValueParameterConstruct)
{
	EXPECT_NO_THROW(ContainerResource(MATRIX_1X1));
	ContainerResource resource(MATRIX_1X1);

	EXPECT_EQ(resource.GetColumnSize(), MATRIX_1X1.size());
	EXPECT_GT(MATRIX_1X1.size(), 0);
	EXPECT_EQ(resource.GetRowSize(), MATRIX_1X1[0].size());
}

TEST(Resource2D, RValueParameterConstruct)
{
	std::vector<std::vector<int>> moveable = MATRIX_1X1;
	EXPECT_NO_THROW(ContainerResource(std::move(moveable)));
	std::vector<std::vector<int>> otherMoveable = MATRIX_1X1;
	ContainerResource resource(std::move(otherMoveable));

	EXPECT_EQ(resource.GetColumnSize(), MATRIX_1X1.size());
	EXPECT_GT(MATRIX_1X1.size(), 0);
	EXPECT_EQ(resource.GetRowSize(), MATRIX_1X1[0].size());
}

TEST(Resource2D, ConstructThrows)
{
	const int rowSize = 1;

	std::vector<std::vector<int>> variableRowsInput;
	variableRowsInput.push_back(std::vector<int>(rowSize));
	variableRowsInput.push_back(std::vector<int>(rowSize+1));

	EXPECT_THROW(ContainerResource resource(variableRowsInput), std::invalid_argument);
}

TEST(Resource2D, MoveConstruct)
{
	ContainerResource source(MATRIX_1X1);
	int sourceChecksum = source.ChecksumProtected();

	// move
	ContainerResource target(std::move(source));

	EXPECT_EQ(*target.Data(), MATRIX_1X1[0][0]);
	EXPECT_EQ(target.ChecksumProtected(), sourceChecksum);
}

TEST(Resource2D, MoveAssign)
{
	ContainerResource source(MATRIX_1X1);
	int sourceChecksum = source.ChecksumProtected();
	ContainerResource target(EMPTY_MATRIX_1X1);

	// move assign
	EXPECT_NO_THROW(target = std::move(source));

	EXPECT_EQ(*target.Data(), MATRIX_1X1[0][0]);
	EXPECT_EQ(target.ChecksumProtected(), sourceChecksum);
}

TEST(Resource2D, CopyConstruct)
{
	ContainerResource source(MATRIX_1X1);

	// copy
	ContainerResource target(source);
	EXPECT_EQ(*target.Data(), MATRIX_1X1[0][0]);
	EXPECT_EQ(target.ChecksumProtected(), source.ChecksumProtected());
}

TEST(Resource2D, CopyAssign)
{
	ContainerResource source(MATRIX_1X1);
	ContainerResource target(EMPTY_MATRIX_1X1);

	// copy assign
	EXPECT_NO_THROW(target = source);

	EXPECT_EQ(*target.Data(), MATRIX_1X1[0][0]);
	EXPECT_EQ(target.ChecksumProtected(), source.ChecksumProtected());
}

TEST(Resource2D, GetDataIJ)
{
	std::vector<std::vector<int>> values = MATRIX_1X1;
	ContainerResource ir(values);

	EXPECT_NO_THROW(ir.GetData(0,0) = VAL);
	values[0][0] = VAL;

	EXPECT_EQ(*ir.Data(), values[0][0]);
}

TEST(Resource2D, GetDataIJConst)
{
	std::vector<std::vector<int>> values = MATRIX_1X1;
	const ContainerResource ir(values);

	EXPECT_NO_THROW(ir.GetData(0, 0));
	const int& val = ir.GetData(0,0);

	EXPECT_EQ(*ir.Data(), val);
}

TEST(Resource2D, GetDataIJThrows)
{
	ContainerResource ir(MATRIX_1X1);
	EXPECT_NO_THROW(ir.GetData(0, 0));
	EXPECT_THROW(ir.GetData(-1, 0), std::invalid_argument);
	EXPECT_THROW(ir.GetData(0, -1), std::invalid_argument);
	EXPECT_THROW(ir.GetData(1, 0), std::invalid_argument);
	EXPECT_THROW(ir.GetData(0, 1), std::invalid_argument);
}

TEST(Resource2D, AssignArray)
{
	ContainerResource ir;
	EXPECT_EQ(MATRIX_1X1.size(), 1);

	ir.SetColumnSize(1);
	ir.SetRowSize(MATRIX_1X1[0].size());

	ir.Assign(reinterpret_cast<const char*>(MATRIX_1X1[0].data()), MATRIX_1X1[0].size() * sizeof(int));
	EXPECT_EQ(*ir.Data(), MATRIX_1X1[0][0]);
}

TEST(Resource2D, AssignMatrix)
{
	ContainerResource ir;
	EXPECT_EQ(BUFFER_2X2.size(), 4);
	ir.SetColumnSize(2);
	ir.SetRowSize(2);

	ir.Assign(reinterpret_cast<const char*>(BUFFER_2X2.data()), BUFFER_2X2.size() * sizeof(int));
	for (int i = 0; i < 4; ++i)
		EXPECT_EQ(*(ir.Data() + i), *(BUFFER_2X2.data() + i));
}

TEST(Resource2D, AssignThrows)
{
	ContainerResource ir;

	// resource dimensions not set
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char*>(MATRIX_1X1[0].data()), MATRIX_1X1.size() * MATRIX_1X1[0].size() * sizeof(int)), std::runtime_error);

	ir.SetColumnSize(MATRIX_1X1.size());
	EXPECT_GT(MATRIX_1X1.size(), 0);
	ir.SetRowSize(MATRIX_1X1[0].size());

	// resource buffer not set
	EXPECT_THROW(ir.Assign(nullptr, MATRIX_1X1.size() * MATRIX_1X1[0].size() * sizeof(int)), std::runtime_error);
	// resource length is 0 
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char*>(MATRIX_1X1[0].data()), 0), std::runtime_error);
	// resource length not congruent with sizeof(T) 
	EXPECT_THROW(ir.Assign(reinterpret_cast<const char*>(MATRIX_1X1[0].data()), sizeof(int)-1), std::runtime_error);
}

TEST(Resource2D, SetColumnSize)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_EQ(ir.GetColumnSize(), 0);
	EXPECT_NO_THROW(ir.SetColumnSize(size));
	EXPECT_EQ(ir.GetColumnSize(), size);
}

TEST(Resource2D, SetColumnSizeThrowsWhenChangingDimension)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_NO_THROW(ir.SetColumnSize(size));
	EXPECT_NO_THROW(ir.SetColumnSize(size));
	EXPECT_THROW(ir.SetColumnSize(size + 1), std::runtime_error);
}

TEST(Resource2D, SetRowSize)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_EQ(ir.GetRowSize(), 0);
	EXPECT_NO_THROW(ir.SetRowSize(size));
	EXPECT_EQ(ir.GetRowSize(), size);
}

TEST(Resource2D, SetRowSizeThrowsWhenChangingDimension)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_NO_THROW(ir.SetRowSize(size));
	EXPECT_NO_THROW(ir.SetRowSize(size));
	EXPECT_THROW(ir.SetRowSize(size + 1), std::runtime_error);
}

TEST(Resource2D, GetColumnSize)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_EQ(ir.GetColumnSize(), 0);
	EXPECT_NO_THROW(ir.SetColumnSize(size));
	EXPECT_EQ(ir.GetColumnSize(), size);
}

TEST(Resource2D, GetRowSize)
{
	const size_t size = 1;
	ContainerResource ir;
	EXPECT_EQ(ir.GetRowSize(), 0);
	EXPECT_NO_THROW(ir.SetRowSize(size));
	EXPECT_EQ(ir.GetRowSize(), size);
}
