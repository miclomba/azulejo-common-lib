#include "DatabaseAdapters/ResourcePersister.h"

#include <stdexcept>
#include <string>
#include <string_view>
#include "Config/filesystem.hpp"

#include "DatabaseAdapters/IPersistableResource.h"
#include "DatabaseAdapters/Sqlite.h"
#include "DatabaseAdapters/SqliteBlob.h"

using database_adapters::IPersistableResource;
using database_adapters::ResourcePersister;
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

ResourcePersister *ResourcePersister::instance_ = nullptr;

ResourcePersister::ResourcePersister() = default;

ResourcePersister::~ResourcePersister()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

ResourcePersister *ResourcePersister::GetInstance()
{
	if (!instance_)
		instance_ = new ResourcePersister();
	return instance_;
}

void ResourcePersister::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void ResourcePersister::OpenDatabase(const Path &dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("ResourcePersister already has a database open");
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

void ResourcePersister::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

Sqlite &ResourcePersister::GetDatabase()
{
	return databaseAdapter_;
}

void ResourcePersister::Persist(const IPersistableResource &resource, const std::string_view key)
{
	if (key.empty())
		throw std::runtime_error("Cannot persist resource with empty key");

	if (!resource.UpdateChecksum())
		return;

	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot persist resource because the database is not open");

	const std::string M = std::to_string(resource.GetColumnSize());
	const std::string N = std::to_string(resource.GetRowSize());
	const std::string SIZE_OF = std::to_string(resource.GetElementSize());

	const std::string sql = "INSERT INTO " + TABLE_NAME + " (" + P_KEY + "," + M_KEY + "," + N_KEY + "," + SIZE_OF_KEY + "," + DATA_KEY + ") VALUES ('" + std::string(key) + "'," + M + "," + N + "," + SIZE_OF + ",?);";
	const size_t size = resource.GetElementSize() * resource.GetColumnSize() * resource.GetRowSize();

	SqliteBlob::InsertBlob(databaseAdapter_, sql, resource.Data(), size);
}

void ResourcePersister::Load(const std::string_view key)
{
	if (key.empty())
		throw std::runtime_error("Cannot unpersist resource with empty key");

	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot unpersist resource because the database is not open");

	std::string sql = "DELETE FROM " + TABLE_NAME + " WHERE " + P_KEY + " = '" + std::string(key) + "';";
	GetDatabase().Execute(sql);
}
