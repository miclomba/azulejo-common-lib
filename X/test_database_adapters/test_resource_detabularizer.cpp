#include "config.h"

#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "ContainerResource.h"
#include "DatabaseAdapters/ITabularizableResource.h"
#include "DatabaseAdapters/ResourceDetabularizer.h"
#include "DatabaseAdapters/ResourceTabularizer.h"

namespace fs = std::filesystem;

using database_adapters::ITabularizableResource;
using database_adapters::ResourceDetabularizer;
using database_adapters::ResourceTabularizer;

namespace
{
const fs::path ROOT_DIR = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY;
const std::string DB_NAME = "db.sqlite";
const fs::path DB_PATH = ROOT_DIR / DB_NAME;
const std::string RESOURCE_KEY = "resource";
const int VAL = 1;
const std::vector<int> ARRAY_1(2, VAL);

using Resource = ContainerResource<int>;

auto RESOURCE_CONSTRUCTOR = []()->std::unique_ptr<ITabularizableResource> { return std::make_unique<Resource>(); };

struct SqliteRemover
{
	SqliteRemover() { RemoveDB(); }
	~SqliteRemover() { RemoveDB(); }

	void RemoveDB()
	{
		if (fs::exists(DB_PATH))
		{
			fs::permissions(DB_PATH, fs::perms::all, fs::perm_options::add);
			fs::remove(DB_PATH);
		}
	}
};

struct ResourceDetabularizerFixture
{
	ResourceDetabularizerFixture(const std::string& key, const std::vector<int>& blob)
	{
		CreateDatabaseTable(blob);
		RegisterResource(key);

		EXPECT_TRUE(fs::exists(DB_PATH));
	}

private:
	void CreateDatabaseTable(const std::vector<int>& blob)
	{
		ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();
		tabularizer->OpenDatabase(DB_PATH);

		Resource resource(ARRAY_1);
		tabularizer->Tabularize(resource, RESOURCE_KEY);

		tabularizer->CloseDatabase();
	}

	void RegisterResource(const std::string& key)
	{
		ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();
		detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	}

	SqliteRemover dbRemover_;
};
} // end namespace anonymous

TEST(ResourceDetabularizer, GetInstance)
{
	EXPECT_NO_THROW(ResourceDetabularizer::GetInstance());
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();
	EXPECT_TRUE(detabularizer);
	EXPECT_NO_THROW(ResourceDetabularizer::ResetInstance());
}

TEST(ResourceDetabularizer, ResetInstance)
{
	EXPECT_NO_THROW(ResourceDetabularizer::ResetInstance());
}

TEST(ResourceDetabularizer, ResetInstanceClosesDatabase)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();
	detabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(ResourceDetabularizer::ResetInstance());

	detabularizer = ResourceDetabularizer::GetInstance();
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(ResourceDetabularizer::ResetInstance());
}

TEST(ResourceDetabularizer, Detabularize)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	ResourceDetabularizerFixture fixture(RESOURCE_KEY, ARRAY_1);

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	// detabularize a resource
	std::unique_ptr<ITabularizableResource> resource;
	EXPECT_NO_THROW(resource = detabularizer->Detabularize(RESOURCE_KEY));

	// verify
	EXPECT_EQ(ARRAY_1.size(), resource->GetColumnSize());
	EXPECT_EQ(size_t(1), resource->GetRowSize());
	EXPECT_EQ(sizeof(int), resource->GetElementSize());
	EXPECT_EQ(sizeof(int) * ARRAY_1.size(), resource->GetColumnSize() * resource->GetRowSize() * resource->GetElementSize());
	EXPECT_EQ(ARRAY_1[0], reinterpret_cast<const int*>(resource->Data())[0]);
	EXPECT_EQ(ARRAY_1[1], reinterpret_cast<const int*>(resource->Data())[1]);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, DetabularizeThrowsWhenDatabaseIsNotOpen)
{
	SqliteRemover remover;

	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();
	detabularizer->CloseDatabase();

	EXPECT_THROW(detabularizer->Detabularize("Foo"), std::runtime_error);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, DetabularizeThrowsWithEmptyKey)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	EXPECT_THROW(detabularizer->Detabularize(""), std::runtime_error);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, GetDatabase)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();
	EXPECT_NO_THROW(detabularizer->GetDatabase());
}

TEST(ResourceDetabularizer, OpenDatabase)
{
	SqliteRemover remover;

	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	detabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(detabularizer->OpenDatabase(DB_PATH), std::runtime_error);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	detabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(detabularizer->CloseDatabase());
	EXPECT_FALSE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, CloseDatabaseWhenDatabaseIsClosed)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(detabularizer->CloseDatabase());

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, RegisterResource)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, RegisterResourceThrowsOnEmptyKey)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_THROW(detabularizer->RegisterResource<int>("", RESOURCE_CONSTRUCTOR), std::runtime_error);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, RegisterResourceThrowsOnExistingKey)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_THROW(detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR), std::runtime_error);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, UnregisterResource)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_NO_THROW(detabularizer->UnregisterResource(RESOURCE_KEY));

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, UnregisterResourceThrowsOnEmptyKey)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_THROW(detabularizer->UnregisterResource(""), std::runtime_error);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, UnregisterResourceThrowsOnUnregisteredKey)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_THROW(detabularizer->UnregisterResource(RESOURCE_KEY), std::runtime_error);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, UnregisterAll)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->HasTabularizationKey(RESOURCE_KEY));
	EXPECT_NO_THROW(detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_TRUE(detabularizer->HasTabularizationKey(RESOURCE_KEY));
	EXPECT_NO_THROW(detabularizer->UnregisterAll());
	EXPECT_FALSE(detabularizer->HasTabularizationKey(RESOURCE_KEY));

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, HasTabularizationKeyFalse)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->HasTabularizationKey(RESOURCE_KEY));

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, HasTabularizationKeyTrue)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_TRUE(detabularizer->HasTabularizationKey(RESOURCE_KEY));

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, GenerateResource)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	std::unique_ptr<ITabularizableResource> resource = detabularizer->GenerateResource(RESOURCE_KEY);
	EXPECT_TRUE(resource);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, GenerateResourceThrowsOnEmptyKey)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_THROW(detabularizer->GenerateResource(""), std::runtime_error);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, GenerateResourceThrowsOnUnregisteredKey)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();

	EXPECT_THROW(detabularizer->GenerateResource(RESOURCE_KEY), std::runtime_error);

	ResourceDetabularizer::ResetInstance();
}

