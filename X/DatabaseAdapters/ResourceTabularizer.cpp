#include "ResourceTabularizer.h"

#include <filesystem>
#include <stdexcept>

#include "ITabularizableResource.h"
#include "Sqlite.h"

using database_adapters::ResourceTabularizer;
using database_adapters::ITabularizableResource;
using database_adapters::Sqlite;

ResourceTabularizer* ResourceTabularizer::instance_ = nullptr;

ResourceTabularizer::ResourceTabularizer() = default;

ResourceTabularizer::~ResourceTabularizer()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

ResourceTabularizer* ResourceTabularizer::GetInstance()
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

void ResourceTabularizer::Tabularize(const ITabularizableResource& resource)
{
	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot tabularize resource because the database is not open");
	resource.Save(GetDatabase());
}

void ResourceTabularizer::OpenDatabase(const std::filesystem::path& dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("ResourceTabularizer already has a database open");
	databaseAdapter_.Open(dbPath);
}

void ResourceTabularizer::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

Sqlite& ResourceTabularizer::GetDatabase()
{
	return databaseAdapter_;
}

