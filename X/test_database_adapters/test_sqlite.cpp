
#include "config.h"

#include <filesystem>

#include <gtest/gtest.h>

#include "DatabaseAdapters/Sqlite.h"

namespace fs = std::filesystem;

using database_adapters::Sqlite;

namespace
{
const std::string DB_NAME = "db.sqlite";

struct SqliteF : public testing::Test
{
	SqliteF() { RemoveDB(); }
	~SqliteF() { RemoveDB(); }

	void RemoveDB()
	{
		if (fs::exists(DB_PATH))
		{
			fs::permissions(
				DB_PATH, 
				fs::perms::owner_all | fs::perms::group_all | fs::perms::others_all,
				fs::perm_options::remove
			);
			fs::remove_all(DB_PATH);
		}
	}

	const fs::path DB_PATH = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY / DB_NAME;
};
} // end namespace

TEST(Sqlite, Construct)
{
	EXPECT_NO_THROW(Sqlite());
}

TEST_F(SqliteF, Open)
{
	Sqlite db;

	EXPECT_FALSE(db.IsOpen());
	EXPECT_NO_THROW(db.Open(DB_PATH));
	EXPECT_TRUE(db.IsOpen());
	db.Close();
}

TEST_F(SqliteF, Close)
{
	Sqlite db;

	db.Open(DB_PATH);
	EXPECT_TRUE(db.IsOpen());
	EXPECT_NO_THROW(db.Close());
	EXPECT_FALSE(db.IsOpen());
}