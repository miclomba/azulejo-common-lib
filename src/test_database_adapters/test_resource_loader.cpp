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

using boost::system::error_code;
using database_adapters::IPersistableResource;
using database_adapters::ResourceLoader;
using database_adapters::ResourcePersister;

namespace
{
	const fs::path ROOT_DIR = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY;
	const std::string DB_NAME = "db.sqlite";
	const fs::path DB_PATH = ROOT_DIR / DB_NAME;
	const std::string RESOURCE_KEY = "resource";
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

	struct ResourceLoaderFixture
	{
		ResourceLoaderFixture(const std::string &key, const std::vector<int> &blob)
		{
			CreateDatabaseTable(blob);
			RegisterResource(key);

			EXPECT_TRUE(fs::exists(DB_PATH));
		}

	private:
		void CreateDatabaseTable(const std::vector<int> &blob)
		{
			ResourcePersister *persister = ResourcePersister::GetInstance();
			persister->OpenDatabase(DB_PATH);

			Resource resource(ARRAY_1);
			persister->Persist(resource, RESOURCE_KEY);

			persister->CloseDatabase();
		}

		void RegisterResource(const std::string &key)
		{
			ResourceLoader *loader = ResourceLoader::GetInstance();
			loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
		}

		SqliteRemover dbRemover_;
	};
} // end namespace anonymous

TEST(ResourceLoader, GetInstance)
{
	EXPECT_NO_THROW(ResourceLoader::GetInstance());
	ResourceLoader *loader = ResourceLoader::GetInstance();
	EXPECT_TRUE(loader);
	EXPECT_NO_THROW(ResourceLoader::ResetInstance());
}

TEST(ResourceLoader, ResetInstance)
{
	EXPECT_NO_THROW(ResourceLoader::ResetInstance());
}

TEST(ResourceLoader, ResetInstanceClosesDatabase)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();
	loader->OpenDatabase(DB_PATH);
	EXPECT_TRUE(loader->GetDatabase().IsOpen());
	EXPECT_NO_THROW(ResourceLoader::ResetInstance());

	loader = ResourceLoader::GetInstance();
	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(ResourceLoader::ResetInstance());
}

TEST(ResourceLoader, Load)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	ResourceLoaderFixture fixture(RESOURCE_KEY, ARRAY_1);

	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));

	// load a resource
	std::unique_ptr<IPersistableResource> resource;
	EXPECT_NO_THROW(resource = loader->Load(RESOURCE_KEY));

	// verify
	EXPECT_EQ(ARRAY_1.size(), resource->GetColumnSize());
	EXPECT_EQ(size_t(1), resource->GetRowSize());
	EXPECT_EQ(sizeof(int), resource->GetElementSize());
	EXPECT_EQ(sizeof(int) * ARRAY_1.size(), resource->GetColumnSize() * resource->GetRowSize() * resource->GetElementSize());
	EXPECT_EQ(ARRAY_1[0], reinterpret_cast<const int *>(resource->Data())[0]);
	EXPECT_EQ(ARRAY_1[1], reinterpret_cast<const int *>(resource->Data())[1]);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, LoadThrowsWhenDatabaseIsNotOpen)
{
	SqliteRemover remover;

	ResourceLoader *loader = ResourceLoader::GetInstance();
	loader->CloseDatabase();

	EXPECT_THROW(loader->Load("Foo"), std::runtime_error);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, LoadThrowsWithEmptyKey)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	EXPECT_THROW(loader->Load(""), std::runtime_error);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, GetDatabase)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();
	EXPECT_NO_THROW(loader->GetDatabase());
}

TEST(ResourceLoader, OpenDatabase)
{
	SqliteRemover remover;

	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_FALSE(loader->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));
	EXPECT_TRUE(loader->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	ResourceLoader *loader = ResourceLoader::GetInstance();

	loader->OpenDatabase(DB_PATH);
	EXPECT_TRUE(loader->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(loader->OpenDatabase(DB_PATH), std::runtime_error);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	ResourceLoader *loader = ResourceLoader::GetInstance();

	loader->OpenDatabase(DB_PATH);
	EXPECT_TRUE(loader->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(loader->CloseDatabase());
	EXPECT_FALSE(loader->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, CloseDatabaseWhenDatabaseIsClosed)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_FALSE(loader->GetDatabase().IsOpen());
	EXPECT_NO_THROW(loader->CloseDatabase());

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, RegisterResource)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_NO_THROW(loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, RegisterResourceThrowsOnEmptyKey)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_THROW(loader->RegisterResource<int>("", RESOURCE_CONSTRUCTOR), std::runtime_error);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, RegisterResourceThrowsOnExistingKey)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_NO_THROW(loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_THROW(loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR), std::runtime_error);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, UnregisterResource)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_NO_THROW(loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_NO_THROW(loader->UnregisterResource(RESOURCE_KEY));

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, UnregisterResourceThrowsOnEmptyKey)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_THROW(loader->UnregisterResource(""), std::runtime_error);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, UnregisterResourceThrowsOnUnregisteredKey)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_THROW(loader->UnregisterResource(RESOURCE_KEY), std::runtime_error);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, UnregisterAll)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_FALSE(loader->HasPersistenceKey(RESOURCE_KEY));
	EXPECT_NO_THROW(loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_TRUE(loader->HasPersistenceKey(RESOURCE_KEY));
	EXPECT_NO_THROW(loader->UnregisterAll());
	EXPECT_FALSE(loader->HasPersistenceKey(RESOURCE_KEY));

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, HasPersistenceKeyFalse)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_FALSE(loader->HasPersistenceKey(RESOURCE_KEY));

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, HasPersistenceKeyTrue)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_NO_THROW(loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_TRUE(loader->HasPersistenceKey(RESOURCE_KEY));

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, GenerateResource)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	loader->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	std::unique_ptr<IPersistableResource> resource = loader->GenerateResource(RESOURCE_KEY);
	EXPECT_TRUE(resource);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, GenerateResourceThrowsOnEmptyKey)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_THROW(loader->GenerateResource(""), std::runtime_error);

	ResourceLoader::ResetInstance();
}

TEST(ResourceLoader, GenerateResourceThrowsOnUnregisteredKey)
{
	ResourceLoader *loader = ResourceLoader::GetInstance();

	EXPECT_THROW(loader->GenerateResource(RESOURCE_KEY), std::runtime_error);

	ResourceLoader::ResetInstance();
}
