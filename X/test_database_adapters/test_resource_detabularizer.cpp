#include "config.h"

#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "DatabaseAdapters/ITabularizableResource.h"
#include "DatabaseAdapters/ResourceDetabularizer.h"
#include "DatabaseAdapters/Sqlite.h"

namespace fs = std::filesystem;

using database_adapters::ITabularizableResource;
using database_adapters::ResourceDetabularizer;
using database_adapters::Sqlite;

namespace
{
const fs::path ROOT_DIR = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY;
const std::string DB_NAME = "db.sqlite";
const fs::path DB_PATH = ROOT_DIR / DB_NAME;
const std::string TABLE_NAME = "tbl";
const std::vector<std::string> COLUMN_NAMES{ "pkey","resourceVal" };
const std::string PKEY = "1";
const std::string VALUE = "1.0";

struct TypeA : public ITabularizableResource
{
	size_t GetElementSize() const override { return 0; }
	void* Data() override { return nullptr; }
	const void* Data() const override { return nullptr; }
	void Assign(const char* buff, const size_t n) override {}
	void Save(Sqlite& database) const override {}

	void Load(Sqlite& database) override
	{
		std::function<int(int, char**, char**)> rowHandler =
			[this](int numCols, char** colValues, char** colNames)
		{
			EXPECT_EQ(COLUMN_NAMES.size(), numCols);
			for (int i = 0; i < numCols; ++i)
			{
				std::string colName = colNames[i];
				EXPECT_EQ(colName, COLUMN_NAMES[i]);
				std::string colValue = colValues[i];
				EXPECT_EQ(colValue, i == 0 ? PKEY : VALUE);

				if (colName == COLUMN_NAMES[1])
					value_ = colValue;
			}
			return 0;
		};

		std::string sql = "SELECT * FROM " + TABLE_NAME + " WHERE " + COLUMN_NAMES[1] + "='" + VALUE + "';";
		database.Execute(sql, rowHandler);
	}

	std::string GetValue() const
	{
		return value_;
	}

private:
	std::string value_;
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

void CreateDatabaseTable(const std::string& value)
{
	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();
	Sqlite& database = detabularizer->GetDatabase();

	database.Open(DB_PATH);

	std::string sql = "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + " (" + COLUMN_NAMES[0] + " INTEGER PRIMARY KEY AUTOINCREMENT, " + COLUMN_NAMES[1] + " TEXT NOT NULL);";
	database.Execute(sql);

	sql = "INSERT INTO " + TABLE_NAME + " (" + COLUMN_NAMES.at(1) + ") VALUES ('" + value + "');";
	database.Execute(sql);

	database.Close();
}

struct ResourceDetabularizerFixture
{
	ResourceDetabularizerFixture(const std::string& value)
	{
		CreateDatabaseTable(value);
		EXPECT_TRUE(fs::exists(DB_PATH));
	}

private:
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

	ResourceDetabularizerFixture fixture(VALUE);

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	// detabularize an resource
	TypeA resource;
	EXPECT_TRUE(resource.GetValue().empty());
	EXPECT_NO_THROW(detabularizer->LoadResource(resource));

	// verify
	EXPECT_EQ(resource.GetValue(), VALUE);

	ResourceDetabularizer::ResetInstance();
}

TEST(ResourceDetabularizer, DetabularizeThrowsWhenDatabaseIsNotOpen)
{
	SqliteRemover remover;

	ResourceDetabularizer* detabularizer = ResourceDetabularizer::GetInstance();
	detabularizer->CloseDatabase();

	TypeA typeA;

	EXPECT_THROW(detabularizer->LoadResource(typeA), std::runtime_error);

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
