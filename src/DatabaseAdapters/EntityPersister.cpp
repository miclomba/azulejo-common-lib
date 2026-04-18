#include "DatabaseAdapters/EntityPersister.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include "Config/filesystem.hpp"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "DatabaseAdapters/EntityHierarchyBlob.h"
#include "DatabaseAdapters/IPersistableEntity.h"
#include "DatabaseAdapters/ResourcePersister.h"
#include "DatabaseAdapters/Sqlite.h"

namespace pt = boost::property_tree;

using database_adapters::EntityHierarchyBlob;
using database_adapters::EntityPersister;
using database_adapters::IPersistableEntity;
using database_adapters::ResourcePersister;
using database_adapters::Sqlite;
using entity::Entity;
using entity::EntityHierarchy;

using Key = Entity::Key;

namespace
{
	const std::string ENTITY_HIERARCHY_BLOB_KEY = "entity_hierarchy_blob";

	void SendEntityHierarchyTreeToDatabase(const pt::ptree &hierarchyTree, Sqlite &database)
	{
		EntityHierarchyBlob hierarchyBlob(hierarchyTree);

		ResourcePersister *resourcePersister = ResourcePersister::GetInstance();

		if (bool resourcePersisterIsOpen = resourcePersister->GetDatabase().IsOpen(); !resourcePersisterIsOpen)
		{
			resourcePersister->OpenDatabase(database.GetPath());
			resourcePersister->Persist(hierarchyBlob, ENTITY_HIERARCHY_BLOB_KEY);
			resourcePersister->CloseDatabase();
		}
		else
		{
			resourcePersister->Persist(hierarchyBlob, ENTITY_HIERARCHY_BLOB_KEY);
		}
	}
}

EntityPersister *EntityPersister::instance_ = nullptr;

EntityPersister::EntityPersister() = default;

EntityPersister::~EntityPersister()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

EntityPersister *EntityPersister::GetInstance()
{
	if (!instance_)
		instance_ = new EntityPersister();
	return instance_;
}

void EntityPersister::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void EntityPersister::Persist(const IPersistableEntity &entity)
{
	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot persist entity because the database is not open");
	if (entity.GetKey().empty())
		throw std::runtime_error("Cannot persist entity because key=" + entity.GetKey() + " is not present in the loaded persistence structure");
	PersistWithParentKey(entity, "");
}

void EntityPersister::PersistWithParentKey(const IPersistableEntity &entity, const std::string_view parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : std::string(parentKey) + "." + entity.GetKey();

	pt::ptree &tree = hierarchy_.GetSerializationStructure().put_child(searchPath, pt::ptree());

	entity.Save(tree, GetDatabase());

	for (auto keys = entity.GetAggregatedMemberKeys<IPersistableEntity>(); const std::string &key : keys)
	{
		Entity::SharedEntity &member = entity.GetAggregatedMember(key);
		auto memberPtr = dynamic_cast<IPersistableEntity *>(member.get());
		PersistWithParentKey(*memberPtr, searchPath);
	}

	if (parentKey.empty())
		SendEntityHierarchyTreeToDatabase(hierarchy_.GetSerializationStructure(), GetDatabase());
}

void EntityPersister::OpenDatabase(const Path &dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("EntityPersister already has a database open");
	databaseAdapter_.Open(dbPath);
}

void EntityPersister::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

Sqlite &EntityPersister::GetDatabase()
{
	return databaseAdapter_;
}

EntityHierarchy &EntityPersister::GetHierarchy()
{
	return hierarchy_;
}
