#include "config.h"

#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "DatabaseAdapters/ITabularizableResource.h"
#include "DatabaseAdapters/ResourceTabularizer.h"
#include "DatabaseAdapters/Sqlite.h"
#include "Entities/Entity.h"

namespace fs = std::filesystem;

using database_adapters::ResourceTabularizer;
using database_adapters::ITabularizableResource;
using database_adapters::Sqlite;

namespace
{
const fs::path ROOT_DIR = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY; 
const std::string DB_NAME = "db.sqlite";
const fs::path DB_PATH = ROOT_DIR / DB_NAME;
const std::string TABLE_NAME = "tbl";
const std::vector<std::string> COLUMN_NAMES{ "pkey","resourceVal" };
const std::string VALUE = "1.0";
const std::string PKEY = "1";

struct TypeA : public ITabularizableResource
{
	size_t GetElementSize() const override { return 0; }
	void* Data() override { return nullptr; }
	const void* Data() const override { return nullptr; }
	void Assign(const char* buff, const size_t n) override {}
	void Load(Sqlite& database) override {}

	void Save(Sqlite& database) const override
	{
		std::string sql = "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + " (" + COLUMN_NAMES[0] + " INTEGER PRIMARY KEY AUTOINCREMENT, " + COLUMN_NAMES[1] + " TEXT NOT NULL);";
		database.Execute(sql);

		sql = "INSERT INTO " + TABLE_NAME + " (" + COLUMN_NAMES.at(1) + ") VALUES ('" + VALUE + "');";
		database.Execute(sql);
	}
};

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
	void VerifyTabularization()
	{
		EXPECT_TRUE(fs::exists(DB_PATH));

		std::function<int(int, char**, char**)> rowHandler =
			[this](int numCols, char** colValues, char** colNames)
		{
			EXPECT_EQ(COLUMN_NAMES.size(), numCols);
			for (int i = 0; i < numCols; ++i)
			{
				std::string colName = colNames[i];
				EXPECT_EQ(colName, COLUMN_NAMES[i]);
				std::string colValue = colValues[i];
				EXPECT_EQ(colValue, colNames[i] == COLUMN_NAMES[0] ? PKEY : VALUE);
			}
			return 0;
		};

		ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

		std::string sql = "SELECT * FROM " + TABLE_NAME + " WHERE " + COLUMN_NAMES[1] + "='" + VALUE + "';";
		tabularizer->GetDatabase().Execute(sql, rowHandler);
	}

	TypeA& GetResource()
	{
		return resource_;
	}

private:
	SqliteRemover dbRemover_;
	TypeA resource_;
};
} // end namespace entity

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

	EXPECT_NO_THROW(tabularizer->Tabularize(fixture.GetResource()));

	fixture.VerifyTabularization();

	ResourceTabularizer::ResetInstance();
}

TEST(ResourceTabularizer, TabularizeThrowsIfDatabaseIsNotOpen)
{
	ResourceTabularizerFixture fixture;

	ResourceTabularizer* tabularizer = ResourceTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	TypeA resource;
	EXPECT_THROW(tabularizer->Tabularize(resource), std::runtime_error);

	ResourceTabularizer::ResetInstance();
}

