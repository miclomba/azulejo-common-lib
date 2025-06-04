#include "DatabaseAdapters/SqliteBlob.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "DatabaseAdapters/Sqlite.h"

#include "sqlite3.h"

using database_adapters::Sqlite;
using database_adapters::SqliteBlob;

namespace
{
	const std::string DB_NAME = "main";
}

SqliteBlob::SqliteBlob(Sqlite &db) : db_(db)
{
}

SqliteBlob::~SqliteBlob()
{
	if (blob_)
		sqlite3_blob_close(blob_);
}

void SqliteBlob::Open(const std::string &tableName, const std::string &columnName, const sqlite3_int64 iRow)
{
	int result = sqlite3_blob_open(
		db_.GetSqlite3(), DB_NAME.c_str(), tableName.c_str(),
		columnName.c_str(), iRow, SQLITE_OPEN_READWRITE, &blob_);

	if (result != SQLITE_OK)
		throw std::runtime_error("Could not open SQLite3 blob at " + tableName + " col=" + columnName + " row=" + std::to_string(iRow));
}

std::vector<char> SqliteBlob::Read(const size_t numBytes, const int offset) const
{
	if (numBytes == 0)
		throw std::runtime_error("SqliteBlob cannot read 0 bytes");
	if (offset < 0)
		throw std::runtime_error("SqliteBlob cannot read at negative offset");

	std::vector<char> buff(numBytes);
	int result = sqlite3_blob_read(blob_, buff.data(), static_cast<int>(numBytes), offset);
	if (result != SQLITE_OK)
		throw std::runtime_error("Could not read " + std::to_string(numBytes) + " from sqlite3 blob at offset=" + std::to_string(offset));
	return buff;
}

void SqliteBlob::Write(const char *buff, const size_t size, const int offset)
{
	if (!buff)
		throw std::runtime_error("SqliteBlob cannot write because buffer is nullptr");
	if (size == 0)
		throw std::runtime_error("SqliteBlob cannot write because buffer length is 0");
	if (offset < 0)
		throw std::runtime_error("SqliteBlob cannot write because offset is negative");

	int result = sqlite3_blob_write(blob_, buff, static_cast<int>(size), offset);
	if (result != SQLITE_OK)
		throw std::runtime_error("Could not write " + std::to_string(size) + " from sqlite3 blob at offset=" + std::to_string(offset));
}

void SqliteBlob::InsertBlob(Sqlite &db, const std::string &sql, const void *data, const size_t size)
{
	if (!data)
		throw std::runtime_error("SqliteBlob could not insert blob because data pointer is nullptr");
	if (size == 0)
		throw std::runtime_error("SqliteBlob could not insert blob because data size is 0");
	if (!db.IsOpen())
		throw std::runtime_error("SqliteBlob could not insert blob because data database is not open");

	sqlite3_stmt *stmt = nullptr;
	int rc = sqlite3_prepare_v2(db.GetSqlite3(), sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		throw std::runtime_error(std::string("Sqlite prepare_v2 failed: ") + sqlite3_errmsg(db.GetSqlite3()));
	}
	else
	{
		// SQLITE_STATIC because the statement is finalized before the buffer is freed:
		rc = sqlite3_bind_blob(stmt, 1, data, size, SQLITE_STATIC);
		if (rc != SQLITE_OK)
		{
			throw std::runtime_error(std::string("Sqlite bind_blob failed: ") + sqlite3_errmsg(db.GetSqlite3()));
		}
		else
		{
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
				throw std::runtime_error(std::string("Sqlite sttement step execution failed: ") + sqlite3_errmsg(db.GetSqlite3()));
		}
	}
	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK)
		throw std::runtime_error(std::string("Sqlite statment finalize failed: ") + sqlite3_errmsg(db.GetSqlite3()));
}
