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
#include "DatabaseAdapters/SqliteBlob.h"

namespace fs = std::filesystem;

using database_adapters::ITabularizableResource;
using database_adapters::ResourceTabularizer;
using database_adapters::ResourceDetabularizer;
using database_adapters::SqliteBlob;

namespace
{
const fs::path ROOT_DIR = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY; 
const std::string DB_NAME = "db.sqlite";
const fs::path DB_PATH = ROOT_DIR / DB_NAME;
const std::string RESOURCE_KEY = "resource";
const std::string DATA_KEY = "data";
const std::string TABLE_NAME = "resources";
const sqlite3_int64 VALID_ROW = 1;
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

struct ResourceTabularizerFixture 
{
	ResourceTabularizerFixture() :
		resource_(ARRAY_1)
	{
		ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();
		detabularizer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	}

	~ResourceTabularizerFixture()
	{
		ResourceDetabularizer::ResetInstance();
	}

	void VerifyTabularization()
	{
		ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();
		detabularizer->OpenDatabase(DB_PATH);
		std::unique_ptr<ITabularizableResource> resource = detabularizer->Detabularize(RESOURCE_KEY);
		detabularizer->CloseDatabase();

		// verify
		EXPECT_EQ(ARRAY_1.size(), resource->GetColumnSize());
		EXPECT_EQ(size_t(1), resource->GetRowSize());
		EXPECT_EQ(sizeof(int), resource->GetElementSize());
		EXPECT_EQ(sizeof(int) * ARRAY_1.size(), resource->GetColumnSize() * resource->GetRowSize() * resource->GetElementSize());
		EXPECT_EQ(ARRAY_1[0], reinterpret_cast<const int*>(resource->Data())[0]);
		EXPECT_EQ(ARRAY_1[1], reinterpret_cast<const int*>(resource->Data())[1]);
	}

	Resource& GetResource()
	{
		return resource_;
	}

private:
	SqliteRemover dbRemover_;
	Resource resource_;
};
} // end namespace anonymous 

TEST(ResourceTabularizer, GetInstance)
{
	EXPECT_NO_THROW(ResourceTabularizer::GetInstance());
	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();
	EXPECT_TRUE(tabularizer);
	EXPECT_NO_THROW(ResourceTabularizer::ResetInstance());
}

TEST(ResourceTabularizer, ResetInstance)
{
	EXPECT_NO_THROW(ResourceTabularizer::ResetInstance());
}

TEST(ResourceTabularizer, ResetInstanceClosesDatabase)
{
	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();
	tabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(ResourceTabularizer::ResetInstance());

	tabularizer = ResourceTabularizer::GetInstance();
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(ResourceTabularizer::ResetInstance());
}

TEST(ResourceTabularizer, GetDatabase)
{
	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();
	EXPECT_NO_THROW(tabularizer->GetDatabase());
}

TEST(ResourceTabularizer, OpenDatabase)
{
	SqliteRemover remover;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	tabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(tabularizer->OpenDatabase(DB_PATH), std::runtime_error);

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	tabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(tabularizer->CloseDatabase());
	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, CloseDatabaseWhenDatabaseIsClosed)
{
	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, Tabularize)
{
	ResourceTabularizerFixture fixture;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));

	EXPECT_NO_THROW(tabularizer->Tabularize(fixture.GetResource(), RESOURCE_KEY));

	fixture.VerifyTabularization();

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, TabularizeThrowsUsingEmptyKey)
{
	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	Resource resource;
	EXPECT_THROW(tabularizer->Tabularize(resource, ""), std::runtime_error);

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, TabularizeThrowsIfDatabaseIsNotOpen)
{
	ResourceTabularizerFixture fixture;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	Resource resource;
	EXPECT_THROW(tabularizer->Tabularize(resource, RESOURCE_KEY), std::runtime_error);

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, TabularizeThrowsIfResourceDataIsEmpty)
{
	ResourceTabularizerFixture fixture;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));

	Resource resource;
	EXPECT_THROW(tabularizer->Tabularize(resource, RESOURCE_KEY), std::runtime_error);

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, Untabularize)
{
	ResourceTabularizerFixture fixture;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));

	tabularizer->Tabularize(fixture.GetResource(), RESOURCE_KEY);

	{
		SqliteBlob blob(tabularizer->GetDatabase());
		EXPECT_NO_THROW(blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW));
	}

	EXPECT_NO_THROW(tabularizer->Untabularize(RESOURCE_KEY));

	{
		SqliteBlob blob(tabularizer->GetDatabase());
		EXPECT_THROW(blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW), std::runtime_error);
	}

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, UntabularizeThrowsWithEmptyKey)
{
	ResourceTabularizerFixture fixture;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_THROW(tabularizer->Untabularize(""), std::runtime_error);

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, UntabularizeThrowsWithUnopenedDatabase)
{
	ResourceTabularizerFixture fixture;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_THROW(tabularizer->Untabularize(RESOURCE_KEY), std::runtime_error);

	ResourceTabularizer::ResetInstance();
}

