#include "test_database_adapters/config.h"

#include <gtest/gtest.h>

#include "DatabaseAdapters/IPersistableResource.h"

using database_adapters::IPersistableResource;

namespace
{
	struct TypeA : public IPersistableResource
	{
		size_t GetElementSize() const override { return 0; }
		void *Data() override { return nullptr; }
		const void *Data() const override { return nullptr; }
		void Assign(const char *buff, const size_t n) override {}
	};
} // end namespace

TEST(IPersistableResource, Construct)
{
	EXPECT_NO_THROW(TypeA persistable);
}
