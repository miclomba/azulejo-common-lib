#include "DatabaseAdapters/ResourceTabularizer.h"

#include <stdexcept>
#include <string>
#include "Config/filesystem.hpp"

#include "DatabaseAdapters/ITabularizableResource.h"
#include "DatabaseAdapters/Sqlite.h"
#include "DatabaseAdapters/SqliteBlob.h"

using database_adapters::ITabularizableResource;
using database_adapters::ResourceTabularizer;
using database_adapters::Sqlite;
using database_adapters::SqliteBlob;

namespace
{
	const std::string TABLE_NAME = "resources";
	const std::string ROW_KEY = "row";
	const std::string P_KEY = "resource_key";
	const std::string M_KEY = "m";
	const std::string N_KEY = "n";
	const std::string SIZE_OF_KEY = "sizeof";
	const std::string DATA_KEY = "data";
}

ResourceTabularizer *ResourceTabularizer::instance_ = nullptr;

ResourceTabularizer::ResourceTabularizer() = default;

ResourceTabularizer::~ResourceTabularizer()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

ResourceTabularizer *ResourceTabularizer::GetInstance()
{
	if (!instance_)
		instance_ = new ResourceTabularizer();
	return instance_;
}

void ResourceTabularizer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void ResourceTabularizer::OpenDatabase(const Path &dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("ResourceTabularizer already has a database open");
	databaseAdapter_.Open(dbPath);

	std::string sql = "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + " (" +
					  ROW_KEY + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
					  P_KEY + " TEXT NOT NULL, " +
					  M_KEY + " INTEGER, " +
					  N_KEY + " INTEGER, " +
					  SIZE_OF_KEY + " INTEGER, " +
					  DATA_KEY + " BLOB);";

	sql += " PRAGMA auto_vacuum = FULL;";

	databaseAdapter_.Execute(sql);
}

void ResourceTabularizer::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

Sqlite &ResourceTabularizer::GetDatabase()
{
	return databaseAdapter_;
}

void ResourceTabularizer::Tabularize(const ITabularizableResource &resource, const std::string &key)
{
	if (key.empty())
		throw std::runtime_error("Cannot tabularize resource with empty key");

	if (!resource.UpdateChecksum())
		return;

	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot tabularize resource because the database is not open");

	const std::string M = std::to_string(resource.GetColumnSize());
	const std::string N = std::to_string(resource.GetRowSize());
	const std::string SIZE_OF = std::to_string(resource.GetElementSize());

	const std::string sql = "INSERT INTO " + TABLE_NAME + " (" + P_KEY + "," + M_KEY + "," + N_KEY + "," + SIZE_OF_KEY + "," + DATA_KEY + ") VALUES ('" + key + "'," + M + "," + N + "," + SIZE_OF + ",?);";
	const size_t size = resource.GetElementSize() * resource.GetColumnSize() * resource.GetRowSize();

	SqliteBlob::InsertBlob(databaseAdapter_, sql, resource.Data(), size);
}

void ResourceTabularizer::Untabularize(const std::string &key)
{
	if (key.empty())
		throw std::runtime_error("Cannot untabularize resource with empty key");

	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot untabularize resource because the database is not open");

	std::string sql = "DELETE FROM " + TABLE_NAME + " WHERE " + P_KEY + " = '" + key + "';";
	GetDatabase().Execute(sql);
}
