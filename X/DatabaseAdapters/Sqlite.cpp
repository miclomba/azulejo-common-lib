#include "Sqlite.h"

#include <filesystem>
#include <stdexcept>
#include <stdio.h>

#include <boost/optional.hpp>

#include "sqlite3.h"

using database_adapters::Sqlite;

namespace fs = std::filesystem;

namespace
{
int RowCallbackWrapper(void* handler, int numCols, char** colValues, char** colNames) 
{
	auto func = reinterpret_cast<Sqlite::RowCallbackType*>(handler);
	return (*func)(numCols, colValues, colNames);
}
}

Sqlite::Sqlite() = default;
Sqlite::Sqlite(const Sqlite&) = default;
Sqlite& Sqlite::operator=(const Sqlite&) = default;
Sqlite::Sqlite(Sqlite&&) = default;
Sqlite& Sqlite::operator=(Sqlite&&) = default;

Sqlite::~Sqlite()
{
	if (db_)
		delete db_;
}

bool Sqlite::IsOpen() const
{
	return db_ != nullptr;
}

void Sqlite::Open(const fs::path& dbPath)
{
	if (dbPath.empty())
		throw std::runtime_error("Could not create (open) SQLite3 DB from empty path");

	int result = sqlite3_open(dbPath.string().c_str(), &db_);
	if (result != SQLITE_OK)
		throw std::runtime_error("Could not create (open) SQLite3 DB at " + dbPath.string());

	dbPath_ = dbPath;
}

void Sqlite::Close()
{
	if (!db_)
		throw std::runtime_error("Cannot close sqlite3 db because given db is nullptr");

	int result = sqlite3_close(db_);
	if (result != SQLITE_OK)
		throw std::runtime_error("Could not create (close) SQLite3 DB");

	db_ = nullptr;
}

fs::path Sqlite::GetPath() const
{
	return dbPath_;
}

void Sqlite::FreeErrorMessage(char* const ec)
{
	sqlite3_free(ec);
}

void Sqlite::Execute(const std::string& sql, boost::optional<Sqlite::RowCallbackType> rowCallback)
{
	if (!IsOpen())
		throw std::runtime_error("Sqlite cannot execute command because database is not open");

	RowCallbackType* callback = nullptr;
	if (rowCallback)
		callback = &(*rowCallback);

	char* error;
	int res = sqlite3_exec(db_, sql.c_str(), RowCallbackWrapper, callback, &error);

	if (res != SQLITE_OK)
	{
		std::string errorStr(error);
		FreeErrorMessage(error);
		throw std::runtime_error("Sqlite could not execute command: '" + sql + "' due to: " + errorStr);
	}
}

sqlite3* Sqlite::GetSqlite3()
{
	return db_;
}
