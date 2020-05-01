
#include "config.h"

#include <filesystem>

#include <gtest/gtest.h>

#include "DatabaseAdapters/Sqlite.h"

namespace fs = std::filesystem;

using database_adapters::Sqlite;

namespace
{
#ifdef _WIN32
const std::string BAD_DB_PATH = "/$$$%%%";
#else
const std::string BAD_DB_PATH = "/";
#endif
const std::string DB_NAME = "db.sqlite";
const std::string TABLE_NAME = "tbl";
const std::vector<std::string> COLUMN_NAMES{ "pkey","skey" };
const std::map<std::string, std::vector<std::string>> TABLE{
	{COLUMN_NAMES[0], std::vector<std::string>{"1","2"}},
	{COLUMN_NAMES[1], std::vector<std::string>{"hello","world"}}
};

struct SqliteF : public testing::Test
{
	SqliteF() { RemoveDB(); }
	~SqliteF() { RemoveDB(); }

	void RemoveDB()
	{
		if (fs::exists(DB_PATH))
		{
			fs::permissions(DB_PATH, fs::perms::all, fs::perm_options::add);
			fs::remove(DB_PATH);
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

	EXPECT_NO_THROW(db.Open(DB_PATH));
	db.Close();
}

TEST_F(SqliteF, OpenThrowsIfDatabasePathDoesNotExist)
{
	Sqlite db;

	EXPECT_THROW(db.Open(BAD_DB_PATH), std::runtime_error);
}

TEST_F(SqliteF, IsOpen)
{
	Sqlite db;

	EXPECT_FALSE(db.IsOpen());
	db.Open(DB_PATH);
	EXPECT_TRUE(db.IsOpen());
	db.Close();
}

TEST_F(SqliteF, Close)
{
	Sqlite db;

	db.Open(DB_PATH);
	EXPECT_NO_THROW(db.Close());
}

TEST_F(SqliteF, IntegrationTest)
{
	Sqlite db;
	db.Open(DB_PATH);

	std::string sql = "CREATE TABLE " + TABLE_NAME + " (" + COLUMN_NAMES[0] + " INTEGER PRIMARY KEY AUTOINCREMENT, " + COLUMN_NAMES[1] + " TEXT NOT NULL);";
	db.Execute(sql);

	auto col = TABLE.find(COLUMN_NAMES[1]);
	const std::string& colName = col->first;
	const std::vector<std::string>& colValues = col->second;
	for (size_t i = 0; i < col->second.size(); ++i)
	{
		sql = "INSERT INTO " + TABLE_NAME + " (" + colName + ") VALUES ('" + colValues.at(i) + "');";
		db.Execute(sql);
	}

	size_t row = 0;
	std::function<int(int, char**, char**)> rowHandler =
		[&row](int numCols, char** colValues, char** colNames)
	{
		EXPECT_EQ(TABLE.size(), numCols);
		for (int i = 0; i < numCols; ++i)
		{
			EXPECT_EQ(colNames[i], COLUMN_NAMES[i]);
			auto col = TABLE.find(COLUMN_NAMES[i]);
			const std::vector<std::string>& columnValues = col->second;
			EXPECT_EQ(colValues[i], columnValues[row]);
		}
		++row;
		return 0;
	};

	sql = "SELECT * FROM " + TABLE_NAME + ";";
	db.Execute(sql, rowHandler);
	db.Close();
}
