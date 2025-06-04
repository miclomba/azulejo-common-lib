
#include "test_database_adapters/config.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include "Config/filesystem.hpp"

#include <gtest/gtest.h>
#include <boost/system/error_code.hpp>

#include "test_database_adapters/ContainerResource.h"
#include "DatabaseAdapters/ResourceTabularizer.h"
#include "DatabaseAdapters/Sqlite.h"
#include "DatabaseAdapters/SqliteBlob.h"

using boost::system::error_code;
using database_adapters::ResourceTabularizer;
using database_adapters::Sqlite;
using database_adapters::SqliteBlob;

namespace
{
	const std::string DB_NAME = "db.sqlite";
	const fs::path DB_PATH = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY / DB_NAME;
	const std::string DATA_KEY = "data";
	const std::string TABLE_NAME = "resources";
	const std::string RESOURCE_KEY = "resource";
	const int VAL = 1;
	const std::vector<int> ARRAY_1(2, VAL);
	const sqlite3_int64 INVALID_ROW = 0;
	const sqlite3_int64 VALID_ROW = 1;
	const size_t INVALID_SIZE = 0;
	const size_t VALID_SIZE = 1;
	const int INVALID_OFFSET = -1;
	const int VALID_OFFSET = 0;
	const std::string ONE_ROW = "1";
	const std::string P_KEY = "resource_key";
	const std::string M_KEY = "m";
	const std::string N_KEY = "n";
	const std::string SIZE_OF_KEY = "sizeof";

	using Resource = ContainerResource<int>;

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

	struct SqliteBlobF : public testing::Test
	{
		SqliteBlobF()
		{
			ResourceTabularizer *tabularizer = ResourceTabularizer::GetInstance();
			tabularizer->OpenDatabase(DB_PATH);

			Resource resource_(ARRAY_1);
			tabularizer->Tabularize(resource_, RESOURCE_KEY);

			tabularizer->CloseDatabase();

			db_.Open(DB_PATH);
		}

		~SqliteBlobF()
		{
			db_.Close();
			ResourceTabularizer::ResetInstance();
		}

		Sqlite &GetDatabase()
		{
			return db_;
		}

	private:
		Sqlite db_;
		SqliteRemover dbRemover_;
	};

	struct SqliteBlobG : public testing::Test
	{
		SqliteBlobG()
		{
			ResourceTabularizer *tabularizer = ResourceTabularizer::GetInstance();
			tabularizer->OpenDatabase(DB_PATH);
			tabularizer->CloseDatabase();

			db_.Open(DB_PATH);
		}

		~SqliteBlobG()
		{
			db_.Close();
			ResourceTabularizer::ResetInstance();
		}

		Sqlite &GetDatabase()
		{
			return db_;
		}

	private:
		Sqlite db_;
		SqliteRemover dbRemover_;
	};
} // end namespace

TEST_F(SqliteBlobF, Construct)
{
	Sqlite &db = GetDatabase();
	EXPECT_NO_THROW(SqliteBlob blob(db));
}

TEST_F(SqliteBlobF, DestructBeforeOpenning)
{
	SqliteBlob blob(GetDatabase());
}

TEST_F(SqliteBlobF, DestructAfterOpenning)
{
	SqliteBlob blob(GetDatabase());
	blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW);
}

TEST_F(SqliteBlobF, Open)
{
	SqliteBlob blob(GetDatabase());

	EXPECT_NO_THROW(blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW));

	std::vector<char> data = blob.Read(ARRAY_1.size() * sizeof(int), VALID_OFFSET);
	EXPECT_EQ(ARRAY_1[0], reinterpret_cast<const int *>(data.data())[0]);
	EXPECT_EQ(ARRAY_1[1], reinterpret_cast<const int *>(data.data())[1]);
}

TEST_F(SqliteBlobF, OpenThrowsWithInvalidTableName)
{
	SqliteBlob blob(GetDatabase());

	EXPECT_THROW(blob.Open("", DATA_KEY, VALID_ROW), std::runtime_error);
}

TEST_F(SqliteBlobF, OpenThrowsWithInvalidColumnName)
{
	SqliteBlob blob(GetDatabase());

	EXPECT_THROW(blob.Open(TABLE_NAME, "", VALID_ROW), std::runtime_error);
}

TEST_F(SqliteBlobF, OpenThrowsWithInvalidRowNumber)
{
	SqliteBlob blob(GetDatabase());

	EXPECT_THROW(blob.Open(TABLE_NAME, DATA_KEY, INVALID_ROW), std::runtime_error);
}

TEST_F(SqliteBlobF, Read)
{
	SqliteBlob blob(GetDatabase());

	blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW);

	std::vector<char> data;
	EXPECT_NO_THROW(data = blob.Read(ARRAY_1.size() * sizeof(int), VALID_OFFSET));
	EXPECT_EQ(ARRAY_1[0], reinterpret_cast<const int *>(data.data())[0]);
	EXPECT_EQ(ARRAY_1[1], reinterpret_cast<const int *>(data.data())[1]);
}

TEST_F(SqliteBlobF, ReadThrowsWithInvalidSize)
{
	SqliteBlob blob(GetDatabase());

	blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW);

	std::vector<char> data;
	EXPECT_THROW(data = blob.Read(INVALID_SIZE, VALID_OFFSET), std::runtime_error);
}

TEST_F(SqliteBlobF, ReadThrowsWithInvalidOffset)
{
	SqliteBlob blob(GetDatabase());

	blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW);

	std::vector<char> data;
	EXPECT_THROW(data = blob.Read(ARRAY_1.size() * sizeof(int), INVALID_OFFSET), std::runtime_error);
}

TEST_F(SqliteBlobF, Write)
{
	SqliteBlob blob(GetDatabase());

	blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW);

	std::vector<int> buff(ARRAY_1.size());
	std::transform(ARRAY_1.cbegin(), ARRAY_1.cend(), buff.begin(), [](int i)
				   { return i + 1; });
	EXPECT_NO_THROW(blob.Write(reinterpret_cast<const char *>(buff.data()), buff.size() * sizeof(int), VALID_OFFSET));

	std::vector<char> data = blob.Read(ARRAY_1.size() * sizeof(int), VALID_OFFSET);
	EXPECT_EQ(ARRAY_1[0] + 1, reinterpret_cast<const int *>(data.data())[0]);
	EXPECT_EQ(ARRAY_1[1] + 1, reinterpret_cast<const int *>(data.data())[1]);
}

TEST_F(SqliteBlobF, WriteThrowsWithNullBuffer)
{
	SqliteBlob blob(GetDatabase());

	blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW);

	EXPECT_THROW(blob.Write(nullptr, VALID_SIZE, VALID_OFFSET), std::runtime_error);
}

TEST_F(SqliteBlobF, WriteThrowsWithInvalidSize)
{
	SqliteBlob blob(GetDatabase());

	blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW);

	std::vector<int> buff(ARRAY_1.size());
	EXPECT_THROW(blob.Write(reinterpret_cast<const char *>(buff.data()), INVALID_SIZE, VALID_OFFSET), std::runtime_error);
}

TEST_F(SqliteBlobF, WriteThrowsWithInvalidOffset)
{
	SqliteBlob blob(GetDatabase());

	blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW);

	std::vector<int> buff(ARRAY_1.size());
	EXPECT_THROW(blob.Write(reinterpret_cast<const char *>(buff.data()), buff.size() * sizeof(int), INVALID_OFFSET), std::runtime_error);
}

TEST_F(SqliteBlobG, InsertBlob)
{
	const std::string sql = "INSERT INTO " + TABLE_NAME + " (" + P_KEY + "," + M_KEY + "," + N_KEY + "," + SIZE_OF_KEY + "," + DATA_KEY + ") VALUES ('" + RESOURCE_KEY + "'," + ONE_ROW + "," + std::to_string(ARRAY_1.size()) + "," + std::to_string(sizeof(int)) + ",?);";
	EXPECT_NO_THROW(SqliteBlob::InsertBlob(GetDatabase(), sql, reinterpret_cast<const void *>(ARRAY_1.data()), ARRAY_1.size() * sizeof(int)));

	SqliteBlob blob(GetDatabase());
	blob.Open(TABLE_NAME, DATA_KEY, VALID_ROW);

	std::vector<char> data = blob.Read(ARRAY_1.size() * sizeof(int), VALID_OFFSET);
	EXPECT_EQ(ARRAY_1[0], reinterpret_cast<const int *>(data.data())[0]);
	EXPECT_EQ(ARRAY_1[1], reinterpret_cast<const int *>(data.data())[1]);
}

TEST_F(SqliteBlobG, InsertBlobThrowsWithInvalidSQL)
{
	EXPECT_THROW(SqliteBlob::InsertBlob(GetDatabase(), "", reinterpret_cast<const void *>(ARRAY_1.data()), ARRAY_1.size() * sizeof(int)), std::runtime_error);
}

TEST_F(SqliteBlobG, InsertBlobThrowsWithInvalidBuffer)
{
	const std::string sql = "INSERT INTO " + TABLE_NAME + " (" + P_KEY + "," + M_KEY + "," + N_KEY + "," + SIZE_OF_KEY + "," + DATA_KEY + ") VALUES ('" + RESOURCE_KEY + "'," + ONE_ROW + "," + std::to_string(ARRAY_1.size()) + "," + std::to_string(sizeof(int)) + ",?);";
	EXPECT_THROW(SqliteBlob::InsertBlob(GetDatabase(), sql, nullptr, ARRAY_1.size() * sizeof(int)), std::runtime_error);
}

TEST_F(SqliteBlobG, InsertBlobThrowsWithInvalidSize)
{
	const std::string sql = "INSERT INTO " + TABLE_NAME + " (" + P_KEY + "," + M_KEY + "," + N_KEY + "," + SIZE_OF_KEY + "," + DATA_KEY + ") VALUES ('" + RESOURCE_KEY + "'," + ONE_ROW + "," + std::to_string(ARRAY_1.size()) + "," + std::to_string(sizeof(int)) + ",?);";
	EXPECT_THROW(SqliteBlob::InsertBlob(GetDatabase(), sql, reinterpret_cast<const void *>(ARRAY_1.data()), INVALID_SIZE), std::runtime_error);
}
