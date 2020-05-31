#include "EntityTabularizer.h"

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "EntityHierarchyBlob.h"
#include "ITabularizableEntity.h"
#include "ResourceTabularizer.h"
#include "Sqlite.h"

namespace pt = boost::property_tree;

using database_adapters::EntityHierarchyBlob;
using database_adapters::EntityTabularizer;
using database_adapters::ITabularizableEntity;
using database_adapters::ResourceTabularizer;
using database_adapters::Sqlite;
using entity::Entity;
using entity::EntityHierarchy;

using Key = Entity::Key;

namespace
{
const std::string ENTITY_HIERARCHY_BLOB_KEY = "entity_hierarchy_blob";

void SendEntityHierarchyTreeToDatabase(const pt::ptree& hierarchyTree, Sqlite& database)
{
	EntityHierarchyBlob hierarchyBlob(hierarchyTree);

	ResourceTabularizer* resourceTabularizer = ResourceTabularizer::GetInstance();

	bool resourceTabularizerIsOpen = resourceTabularizer->GetDatabase().IsOpen();

	if (!resourceTabularizerIsOpen)
	{
		resourceTabularizer->OpenDatabase(database.GetPath());
		resourceTabularizer->Tabularize(hierarchyBlob, ENTITY_HIERARCHY_BLOB_KEY);
		resourceTabularizer->CloseDatabase();
	}
	else
	{
		resourceTabularizer->Tabularize(hierarchyBlob, ENTITY_HIERARCHY_BLOB_KEY);
	}
}
}

EntityTabularizer* EntityTabularizer::instance_ = nullptr;

EntityTabularizer::EntityTabularizer() = default;

EntityTabularizer::~EntityTabularizer()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

EntityTabularizer* EntityTabularizer::GetInstance()
{
	if (!instance_)
		instance_ = new EntityTabularizer();
	return instance_;
}

void EntityTabularizer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void EntityTabularizer::Tabularize(const ITabularizableEntity& entity)
{
	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot tabularize entity because the database is not open");
	if (entity.GetKey().empty())
		throw std::runtime_error("Cannot tabularize entity because key=" + entity.GetKey() + " is not present in the loaded tabularization structure");
	TabularizeWithParentKey(entity, "");
}

void EntityTabularizer::TabularizeWithParentKey(const ITabularizableEntity& entity, const Key& parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();
	
	pt::ptree& tree = hierarchy_.GetSerializationStructure().put_child(searchPath, pt::ptree());

	entity.Save(tree, GetDatabase());

	std::vector<std::string> keys = entity.GetAggregatedMemberKeys<ITabularizableEntity>();
	for (const std::string& key : keys)
	{
		Entity::SharedEntity& member = entity.GetAggregatedMember(key);
		auto memberPtr = dynamic_cast<ITabularizableEntity*>(member.get());
		TabularizeWithParentKey(*memberPtr,searchPath);
	}

	if (parentKey.empty())
		SendEntityHierarchyTreeToDatabase(hierarchy_.GetSerializationStructure(), GetDatabase());
}

void EntityTabularizer::OpenDatabase(const std::filesystem::path& dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("EntityTabularizer already has a database open");
	databaseAdapter_.Open(dbPath);
}

void EntityTabularizer::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

Sqlite& EntityTabularizer::GetDatabase()
{
	return databaseAdapter_;
}

EntityHierarchy& EntityTabularizer::GetHierarchy()
{
	return hierarchy_;
}

