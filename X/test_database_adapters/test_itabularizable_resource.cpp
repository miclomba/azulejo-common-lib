#include "config.h"

#include <gtest/gtest.h>

#include "DatabaseAdapters/ITabularizableResource.h"
#include "DatabaseAdapters/Sqlite.h"

using database_adapters::ITabularizableResource;
using database_adapters::Sqlite;

namespace
{
const size_t SIZE = 0;

struct TypeA : public ITabularizableResource
{
	size_t GetElementSize() const override { return SIZE; }
	void* Data() override { return nullptr; }
	const void* Data() const override { return nullptr; }
	void Assign(const char* buff, const size_t n) override {}

	void Save(Sqlite& tree) const override {}
	void Load(Sqlite& tree) override {}
};
} // end namespace

TEST(ITabularizableResource, Save)
{
	Sqlite database;

	TypeA tabularizable;
	EXPECT_NO_THROW(tabularizable.Save(database));
}

TEST(ITabularizableResource, Load)
{
	Sqlite database;

	TypeA tabularizable;
	EXPECT_NO_THROW(tabularizable.Load(database));
}
