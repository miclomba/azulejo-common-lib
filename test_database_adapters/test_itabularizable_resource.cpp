#include "config.h"

#include <gtest/gtest.h>

#include "DatabaseAdapters/ITabularizableResource.h"

using database_adapters::ITabularizableResource;

namespace
{
struct TypeA : public ITabularizableResource
{
	size_t GetElementSize() const override { return 0; }
	void* Data() override { return nullptr; }
	const void* Data() const override { return nullptr; }
	void Assign(const char* buff, const size_t n) override {}
};
} // end namespace

TEST(ITabularizableResource, Construct)
{
	EXPECT_NO_THROW(TypeA tabularizable);
}

