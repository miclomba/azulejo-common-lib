
#include "test_resources/config.h"

#include <vector>
#include <gtest/gtest.h>

#include "Resources/IResource.h"

namespace
{
	const size_t SIZE = 1;
	const int VAL = 1;
	const std::vector<int> ARRAY_1(1, VAL);

	struct Resource : resource::IResource
	{
		Resource() = default;

		Resource(const std::vector<int> &values) : data_(values)
		{
			SetRowSize(data_.size());
			SetColumnSize(1);
		}

		void SetColumnSizeProtected(const size_t size)
		{
			SetColumnSize(size);
		}

		void SetRowSizeProtected(const size_t size)
		{
			SetRowSize(size);
		}

		size_t GetElementSize() const override
		{
			return SIZE;
		}

		void *Data() override
		{
			return data_.data();
		}

		const void *Data() const override
		{
			return data_.data();
		}

		void Assign(const char *buff, const size_t n) override
		{
		}

		bool UpdateChecksumProtected() { return UpdateChecksum(); }
		int ChecksumProtected() { return Checksum(); }

	private:
		std::vector<int> data_;
	};
} // end namespace

TEST(IResource, Construct)
{
	EXPECT_NO_THROW(Resource());
}

TEST(IResource, SetColumnSize)
{
	Resource resource;
	EXPECT_NO_THROW(resource.SetColumnSizeProtected(SIZE));
	EXPECT_EQ(resource.GetColumnSize(), SIZE);
}

TEST(IResource, GetColumnSize)
{
	Resource resource;
	resource.SetColumnSizeProtected(SIZE);
	EXPECT_EQ(resource.GetColumnSize(), SIZE);
}

TEST(IResource, SetRowSize)
{
	Resource resource;
	EXPECT_NO_THROW(resource.SetRowSizeProtected(SIZE));
	EXPECT_EQ(resource.GetRowSize(), SIZE);
}

TEST(IResource, GetRowSize)
{
	Resource resource;
	resource.SetRowSizeProtected(SIZE);
	EXPECT_EQ(resource.GetRowSize(), SIZE);
}

TEST(IResource, GetElementSize)
{
	Resource resource;
	EXPECT_EQ(resource.GetElementSize(), SIZE);
}

TEST(IResource, Data)
{
	Resource resource(ARRAY_1);
	EXPECT_EQ(*static_cast<int *>(resource.Data()), VAL);
}

TEST(IResource, DataConst)
{
	const Resource resource(ARRAY_1);
	EXPECT_EQ(*static_cast<const int *>(resource.Data()), VAL);
}

TEST(IResource, Assign)
{
	Resource resource;
	EXPECT_NO_THROW(resource.Assign(nullptr, SIZE));
}

TEST(IResource, UpdateChecksum)
{
	Resource ir(ARRAY_1);

	EXPECT_TRUE(ir.UpdateChecksumProtected());
	EXPECT_FALSE(ir.UpdateChecksumProtected());

	*static_cast<int *>(ir.Data()) = 2 * ARRAY_1[0];
	EXPECT_TRUE(ir.UpdateChecksumProtected());
}

TEST(IResource, Checksum)
{
	Resource ir(ARRAY_1);

	int checksum = ir.ChecksumProtected();
	EXPECT_EQ(ir.ChecksumProtected(), checksum);

	*static_cast<int *>(ir.Data()) = 2 * ARRAY_1[0];
	EXPECT_NE(ir.ChecksumProtected(), checksum);
}

TEST(IResource, GetDirty)
{
	Resource ir(ARRAY_1);

	EXPECT_TRUE(ir.GetDirty());
	EXPECT_TRUE(ir.UpdateChecksumProtected());
	EXPECT_TRUE(ir.GetDirty());
	EXPECT_FALSE(ir.UpdateChecksumProtected());
	EXPECT_FALSE(ir.GetDirty());

	*static_cast<int *>(ir.Data()) = 2 * ARRAY_1[0];
	EXPECT_FALSE(ir.GetDirty());

	EXPECT_TRUE(ir.UpdateChecksumProtected());
	EXPECT_TRUE(ir.GetDirty());
}
