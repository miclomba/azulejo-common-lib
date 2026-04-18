#include "DatabaseAdapters/ResourceLoader.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include "Config/filesystem.hpp"

#include "DatabaseAdapters/IPersistableResource.h"
#include "DatabaseAdapters/Sqlite.h"
#include "DatabaseAdapters/SqliteBlob.h"

using database_adapters::IPersistableResource;
using database_adapters::ResourceLoader;
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

ResourceLoader *ResourceLoader::instance_ = nullptr;

ResourceLoader::ResourceLoader() = default;
ResourceLoader::~ResourceLoader()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

ResourceLoader *ResourceLoader::GetInstance()
{
	if (!instance_)
		instance_ = new ResourceLoader();
	return instance_;
}

void ResourceLoader::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void ResourceLoader::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

void ResourceLoader::OpenDatabase(const Path &dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("ResourceLoader already has a database set");
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

Sqlite &ResourceLoader::GetDatabase()
{
	return databaseAdapter_;
}

void ResourceLoader::UnregisterResource(const std::string_view key)
{
	if (key.empty())
		throw std::runtime_error("Key is empty when unregistering resource with ResourceLoader");
	if (keyToResourceMap_.find(key) == keyToResourceMap_.cend())
		throw std::runtime_error("Key not already registered with the ResourceLoader");

	keyToResourceMap_.erase(std::string(key));
}

bool ResourceLoader::HasPersistenceKey(const std::string_view key) const
{
	return keyToResourceMap_.find(key) != keyToResourceMap_.cend();
}

void ResourceLoader::UnregisterAll()
{
	keyToResourceMap_.clear();
}

std::unique_ptr<IPersistableResource> ResourceLoader::Load(const std::string_view key)
{
	if (key.empty())
		throw std::runtime_error("Key is empty when loading resource with ResourceLoader");

	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot load resource because the database is not open");

	const std::string sql = "SELECT " + P_KEY + ", " + ROW_KEY + ", " + M_KEY + ", " + N_KEY + ", " + SIZE_OF_KEY + " FROM " + TABLE_NAME + " WHERE " + P_KEY + " = '" + std::string(key) + "';";

	int iRow = -1;
	size_t m, n, sizeOf;
	std::function<int(int, char **, char **)> PKeyHandler =
		[&iRow, &m, &n, &sizeOf](int numCols, char **colValues, char **colNames)
	{
		iRow = std::stoi(std::string(colValues[1]));
		m = std::stoi(std::string(colValues[2]));
		n = std::stoi(std::string(colValues[3]));
		sizeOf = std::stoi(std::string(colValues[4]));
		return 0;
	};

	databaseAdapter_.Execute(sql, PKeyHandler);

	// TODO not very efficient since here we do a second query to DB
	SqliteBlob sqliteBlob(GetDatabase());
	sqliteBlob.Open(TABLE_NAME, DATA_KEY, iRow);

	size_t size = m * n * sizeOf;
	std::vector<char> blob = sqliteBlob.Read(size, 0);

	std::unique_ptr<IPersistableResource> resource = GenerateResource(key);
	resource->SetRowSize(m);
	resource->SetColumnSize(n);
	resource->Assign(blob.data(), size);

	return resource;
}

std::unique_ptr<IPersistableResource> ResourceLoader::GenerateResource(const std::string_view key) const
{
	if (key.empty())
		throw std::runtime_error("Key is empty when generating resource with ResourceLoader");
	if (keyToResourceMap_.find(key) == keyToResourceMap_.cend())
		throw std::runtime_error("Key is not registered with the ResourceLoader");

	std::unique_ptr<IPersistableResource> resource = keyToResourceMap_[std::string(key)]();

	return resource;
}
