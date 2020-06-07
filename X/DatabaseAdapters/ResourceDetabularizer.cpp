#include "ResourceDetabularizer.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "ITabularizableResource.h"
#include "Sqlite.h"
#include "SqliteBlob.h"

namespace fs = std::filesystem;

using database_adapters::ResourceDetabularizer;
using database_adapters::ITabularizableResource;
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

ResourceDetabularizer* ResourceDetabularizer::instance_ = nullptr;

ResourceDetabularizer::ResourceDetabularizer() = default;
ResourceDetabularizer::~ResourceDetabularizer()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

ResourceDetabularizer* ResourceDetabularizer::GetInstance()
{
	if (!instance_)
		instance_ = new ResourceDetabularizer();
	return instance_;
}

void ResourceDetabularizer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void ResourceDetabularizer::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

void ResourceDetabularizer::OpenDatabase(const std::filesystem::path& dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("ResourceDetabularizer already has a database set");
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

Sqlite& ResourceDetabularizer::GetDatabase()
{
	return databaseAdapter_;
}

void ResourceDetabularizer::UnregisterResource(const std::string& key)
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when unregistering resource with ResourceDetabularizer");
	if (keyToResourceMap_.find(key) == keyToResourceMap_.cend())
		throw std::runtime_error("Key=" + key + " not already registered with the ResourceDetabularizer");

	keyToResourceMap_.erase(key);
}

bool ResourceDetabularizer::HasTabularizationKey(const std::string& key) const
{
	return keyToResourceMap_.find(key) != keyToResourceMap_.cend();
}

void ResourceDetabularizer::UnregisterAll()
{
	keyToResourceMap_.clear();
}

std::unique_ptr<ITabularizableResource> ResourceDetabularizer::Detabularize(const std::string& key)
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when detabularizing resource with ResourceDetabularizer");

	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot detabularize resource because the database is not open");

	const std::string sql = "SELECT " + P_KEY + ", " + ROW_KEY + ", " + M_KEY + ", " + N_KEY + ", " + SIZE_OF_KEY + " FROM " + TABLE_NAME + " WHERE " + P_KEY + " = '" + key + "';";

	int iRow = -1;
	size_t m, n, sizeOf;
	std::function<int(int, char**, char**)> PKeyHandler =
		[&iRow, &m, &n, &sizeOf](int numCols, char** colValues, char** colNames)
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
	
	std::unique_ptr<ITabularizableResource> resource = GenerateResource(key);
	resource->SetRowSize(m);
	resource->SetColumnSize(n);
	resource->Assign(blob.data(), size);

	return resource;
}

std::unique_ptr<ITabularizableResource> ResourceDetabularizer::GenerateResource(const std::string& key) const
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when generating resource with ResourceDetabularizer");
	if (keyToResourceMap_.find(key) == keyToResourceMap_.cend())
		throw std::runtime_error("Key=" + key + " is not registered with the ResourceDetabularizer");

	std::unique_ptr<ITabularizableResource> resource = keyToResourceMap_[key]();

	return resource;
}

