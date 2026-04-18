#include "test_database_adapters/config.h"

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include "Config/filesystem.hpp"

#include <gtest/gtest.h>
#include <boost/system/error_code.hpp>

#include "test_database_adapters/ContainerResource.h"
#include "DatabaseAdapters/IPersistableResource.h"
#include "DatabaseAdapters/ResourceLoader.h"
#include "DatabaseAdapters/ResourcePersister.h"
#include "DatabaseAdapters/SqliteBlob.h"

using boost::system::error_code;
using database_adapters::IPersistableResource;
using database_adapters::ResourceLoader;
using database_adapters::ResourcePersister;
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

	auto RESOURCE_CONSTRUCTOR = []() -> std::unique_ptr<IPersistableResource>
	{ return std::make_unique<Resource>(); };

	struct SqliteRemover
	{
		SqliteRemover() { RemoveDB(); }
		~SqliteRemover() { RemoveDB(); }

		void RemoveDB()
		{
			if (fs::exists(DB_PATH))
			{
#if defined(__APPLE__) || defined(__MACH__)
				error_code ec;
				fs::permissions(DB_PATH, fs::perms::all_all, ec);
#else
				fs::permissions(DB_PATH, fs::perms::all, fs::perm_options::add);
#endif
				fs::remove(DB_PATH);
			}
		}
	};

	struct ResourcePersisterFixture
	{
		ResourcePersisterFixture() : resource_(ARRAY_1)
		{
			ResourceLoader *loader = ResourceLoader::GetInstance();
			loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
		}

		~ResourcePersisterFixture()
		{
			ResourceLoader::ResetInstance();
		}

		void VerifyPersistence()
		{
			ResourceLoader *loader = ResourceLoader::GetInstance();
			loader->OpenDatabase(DB_PATH);
			std::unique_ptr<IPersistableResource> resource = loader->Load(RESOURCE_KEY);
			loader->CloseDatabase();

			// verify
			EXPECT_EQ(ARRAY_1.size(), resource->GetColumnSize());
			EXPECT_EQ(size_t(1), resource->GetRowSize());
			EXPECT_EQ(sizeof(int), resource->GetElementSize());
			EXPECT_EQ(sizeof(int) * ARRAY_1.size(), resource->GetColumnSize() * resource->GetRowSize() * resource->GetElementSize());
			EXPECT_EQ(ARRAY_1[0], reinterpret_cast<const int *>(resource->Data())[0]);
			EXPECT_EQ(ARRAY_1[1], reinterpret_cast<const int *>(resource->Data())[1]);
		}

		Resource &GetResource()
		{
			return resource_;
		}

	private:
		SqliteRemover dbRemover_;
		Resource resource_;
	};
} // end namespace anonymous

TEST(ResourcePersister, GetInstance)
{
	EXPECT_NO_THROW(ResourcePersister::GetInstance());
	ResourcePersister *persister = ResourcePersister::GetInstance();
	EXPECT_TRUE(persister);
	EXPECT_NO_THROW(ResourcePersister::ResetInstance());
}

TEST(ResourcePersister, ResetInstance)
{
	EXPECT_NO_THROW(ResourcePersister::ResetInstance());
}

TEST(ResourcePersister, ResetInstanceClosesDatabase)
{
	ResourcePersister *persister = ResourcePersister::GetInstance();
	persister->OpenDatabase(DB_PATH);
	EXPECT_TRUE(persister->GetDatabase().IsOpen());
	EXPECT_NO_THROW(ResourcePersister::ResetInstance());

	persister = ResourcePersister::GetInstance();
	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(ResourcePersister::ResetInstance());
}

TEST(ResourcePersister, GetDatabase)
{
	ResourcePersister *persister = ResourcePersister::GetInstance();
	EXPECT_NO_THROW(persister->GetDatabase());
}

TEST(ResourcePersister, OpenDatabase)
{
	SqliteRemover remover;

	ResourcePersister *persister = ResourcePersister::GetInstance();

	EXPECT_FALSE(persister->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	EXPECT_TRUE(persister->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	ResourcePersister *persister = ResourcePersister::GetInstance();

	persister->OpenDatabase(DB_PATH);
	EXPECT_TRUE(persister->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(persister->OpenDatabase(DB_PATH), std::runtime_error);

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	ResourcePersister *persister = ResourcePersister::GetInstance();

	persister->OpenDatabase(DB_PATH);
	EXPECT_TRUE(persister->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(persister->CloseDatabase());
	EXPECT_FALSE(persister->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, CloseDatabaseWhenDatabaseIsClosed)
{
	ResourcePersister *persister = ResourcePersister::GetInstance();

	EXPECT_FALSE(persister->GetDatabase().IsOpen());
	EXPECT_NO_THROW(persister->CloseDatabase());

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, Persist)
{
	ResourcePersisterFixture fixture;

	ResourcePersister *persister = ResourcePersister::GetInstance();

	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));

	EXPECT_NO_THROW(persister->Persist(fixture.GetResource(), RESOURCE_KEY));

	fixture.VerifyPersistence();

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, PersistThrowsUsingEmptyKey)
{
	ResourcePersister *persister = ResourcePersister::GetInstance();

	Resource resource;
	EXPECT_THROW(persister->Persist(resource, ""), std::runtime_error);

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, PersistThrowsIfDatabaseIsNotOpen)
{
	ResourcePersisterFixture fixture;

	ResourcePersister *persister = ResourcePersister::GetInstance();

	EXPECT_NO_THROW(persister->CloseDatabase());

	Resource resource;
	EXPECT_THROW(persister->Persist(resource, RESOURCE_KEY), std::runtime_error);

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, PersistThrowsIfResourceDataIsEmpty)
{
	ResourcePersisterFixture fixture;

	ResourcePersister *persister = ResourcePersister::GetInstance();

	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));

	Resource resource;
	EXPECT_THROW(persister->Persist(resource, RESOURCE_KEY), std::runtime_error);

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, Load)
{
	ResourcePersisterFixture fixture;

	ResourcePersister *persister = ResourcePersister::GetInstance();

	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));

	persister->Persist(fixture.GetResource(), RESOURCE_KEY);

	{
		SqliteBlob blob(persister->GetDatabase());
		EXPECT_NO_THROW(blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW));
	}

	EXPECT_NO_THROW(persister->Load(RESOURCE_KEY));

	{
		SqliteBlob blob(persister->GetDatabase());
		EXPECT_THROW(blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW), std::runtime_error);
	}

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, LoadThrowsWithEmptyKey)
{
	ResourcePersisterFixture fixture;

	ResourcePersister *persister = ResourcePersister::GetInstance();

	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	EXPECT_THROW(persister->Load(""), std::runtime_error);

	ResourcePersister::ResetInstance();
}

TEST(ResourcePersister, LoadThrowsWithUnopenedDatabase)
{
	ResourcePersisterFixture fixture;

	ResourcePersister *persister = ResourcePersister::GetInstance();

	EXPECT_FALSE(persister->GetDatabase().IsOpen());
	EXPECT_THROW(persister->Load(RESOURCE_KEY), std::runtime_error);

	ResourcePersister::ResetInstance();
}
