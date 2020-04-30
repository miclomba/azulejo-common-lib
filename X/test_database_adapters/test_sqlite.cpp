
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

TEST_F(SqliteF, Create)
{
	Sqlite db;

	db.Open(DB_PATH);
	db.Close();
}

TEST_F(SqliteF, CreateTable)
{
	Sqlite db;

	std::string sql = "CREATE TABLE IF NOT EXISTS GRADES( \
		ID		INTEGER		PRIMARY KEY AUTOINCREMENT, \
		NAME	TEXT		NOT NULL, \
		LNAME	TEXT		NOT NULL, \
		AGE		INT			NOT NULL, \
		ADDRESS	CHAR(50), \
		GRADE	CHAR(1) );";

	db.Open(DB_PATH);
	db.Execute(sql);
	db.Close();
}

TEST_F(SqliteF, InsertData)
{
	Sqlite db;

	std::string sql = 
		"INSERTO INTO GRADES (NAME LNAME AGE ADDRESS GRADE) VALUES ('Alice', 'Chapa', 35, 'Tampa', 'A'); \
		 INSERTO INTO GRADES (NAME LNAME AGE ADDRESS GRADE) VALUES ('Bob', 'Lee', 20, 'Dallas', 'B'); \
		 INSERTO INTO GRADES (NAME LNAME AGE ADDRESS GRADE) VALUES ('Fred', 'Cooper', 24, 'New York', 'C'); ";

	db.Open(DB_PATH);
	db.Execute(sql);
	db.Close();
}

TEST_F(SqliteF, Select)
{
	Sqlite db;

	std::string sql = "SELECT * FROM GRADES;";

	db.Open(DB_PATH);

	std::function<int(int,char**,char**)> rowHandler = 
		[](int numCols, char** colValues, char** colNames) 
	{
		for (int i = 0; i < numCols; ++i)
			std::cout << colNames[i] << "\t = " << colValues[i];
		std::cout << std::endl;

		return 0;
	};

	db.Execute(sql, rowHandler);
	db.Close();
}