#include "Sqlite.h"

#include <filesystem>
#include <stdexcept>

#include "sqlite3.h"

using database_adapters::Sqlite;

namespace fs = std::filesystem;

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
	int result = sqlite3_open(dbPath.string().c_str(), &db_);
	if (result != 0)
		throw std::runtime_error("Could not create (open) SQLite3 DB at " + dbPath.string());
}

void Sqlite::Close()
{
	if (!db_)
		throw std::runtime_error("Cannot close sqlite3 db because given db is nullptr");

	int result = sqlite3_close(db_);
	if (result != 0)
		throw std::runtime_error("Could not create (close) SQLite3 DB");

	db_ = nullptr;
}

