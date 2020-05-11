#include "ResourceDetabularizer.h"

#include <filesystem>
#include <stdexcept>

#include "ITabularizableResource.h"
#include "Sqlite.h"

using database_adapters::ResourceDetabularizer;
using database_adapters::ITabularizableResource;
using database_adapters::Sqlite;

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
}

Sqlite& ResourceDetabularizer::GetDatabase()
{
	return databaseAdapter_;
}

void ResourceDetabularizer::LoadResource(ITabularizableResource& resource)
{
	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot detabularize resource because the database is not open");
	resource.Load(GetDatabase());
}

