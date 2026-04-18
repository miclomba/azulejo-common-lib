#include "DatabaseAdapters/EntityLoader.h"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include "Config/filesystem.hpp"

#include <boost/property_tree/ptree.hpp>

#include "Entities/Entity.h"
#include "Entities/EntityHierarchy.h"
#include "Entities/EntityRegistry.h"
#include "DatabaseAdapters/EntityHierarchyBlob.h"
#include "DatabaseAdapters/IPersistableEntity.h"
#include "DatabaseAdapters/IPersistableResource.h"
#include "DatabaseAdapters/ResourceLoader.h"
#include "DatabaseAdapters/Sqlite.h"

namespace pt = boost::property_tree;

using database_adapters::EntityLoader;
using database_adapters::EntityHierarchyBlob;
using database_adapters::IPersistableEntity;
using database_adapters::IPersistableResource;
using database_adapters::ResourceLoader;
using database_adapters::Sqlite;
using entity::Entity;
using entity::EntityHierarchy;
using entity::EntityRegistry;

using Key = Entity::Key;

namespace
{
	const std::string ENTITY_HIERARCHY_BLOB_KEY = "entity_hierarchy_blob";

	auto ENTITY_HIERARCHY_BLOB_CONSTRUCTOR = []() -> std::unique_ptr<IPersistableResource>
	{
		return std::make_unique<EntityHierarchyBlob>();
	};

	std::string GetKeyPath(const std::string_view key, const pt::ptree &tree)
	{
		for (const std::pair<std::string, pt::ptree> &keyValue : tree)
		{
			auto& [nodeKey, node] = keyValue;
			if (nodeKey == key)
			{
				return nodeKey;
			}
			else
			{
				std::string returnedKey = GetKeyPath(key, node);
				size_t pos = returnedKey.find_last_of('.');
				if ((pos == std::string::npos && returnedKey == key) || (pos != std::string::npos && returnedKey.substr(pos + 1) == key))
					return nodeKey + "." + returnedKey;
			}
		}
		return "";
	}

	std::string GetParentKeyPath(const std::string_view keyPath)
	{
		std::string parentKeyPath;

		if (size_t pos = keyPath.find_last_of('.'); pos != std::string::npos)
			parentKeyPath = keyPath.substr(0, pos);

		return parentKeyPath;
	}

	pt::ptree GetEntityHierarchyTree(const Sqlite &database)
	{
		ResourceLoader *resourceLoader = ResourceLoader::GetInstance();

		bool resourceLoaderIsOpen = resourceLoader->GetDatabase().IsOpen();

		if (!resourceLoaderIsOpen)
			resourceLoader->OpenDatabase(database.GetPath());
		if (!resourceLoader->HasPersistenceKey(ENTITY_HIERARCHY_BLOB_KEY))
			resourceLoader->RegisterResource<char>(ENTITY_HIERARCHY_BLOB_KEY, ENTITY_HIERARCHY_BLOB_CONSTRUCTOR);

		std::unique_ptr<IPersistableResource> entityHierarchyBlob;
		try
		{
			entityHierarchyBlob = resourceLoader->Load(ENTITY_HIERARCHY_BLOB_KEY);
		}
		catch (std::runtime_error &)
		{
			if (!resourceLoaderIsOpen)
				resourceLoader->CloseDatabase();
			return pt::ptree();
		}

		if (!resourceLoaderIsOpen)
			resourceLoader->CloseDatabase();

		auto entityHierarchyBlobPtr = dynamic_cast<EntityHierarchyBlob *>(entityHierarchyBlob.get());
		return entityHierarchyBlobPtr->GetHierarchyTree();
	}
} // end namespace anonymous

EntityLoader *EntityLoader::instance_ = nullptr;

EntityLoader::EntityLoader() = default;
EntityLoader::~EntityLoader()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

EntityLoader *EntityLoader::GetInstance()
{
	if (!instance_)
		instance_ = new EntityLoader();
	return instance_;
}

void EntityLoader::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

EntityRegistry<IPersistableEntity> &EntityLoader::GetRegistry()
{
	return registry_;
}

EntityHierarchy &EntityLoader::GetHierarchy()
{
	return hierarchy_;
}

void EntityLoader::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

void EntityLoader::OpenDatabase(const Path &dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("EntityLoader already has a database set");
	databaseAdapter_.Open(dbPath);

	hierarchy_.GetSerializationStructure() = GetEntityHierarchyTree(GetDatabase());
}

Sqlite &EntityLoader::GetDatabase()
{
	return databaseAdapter_;
}

void EntityLoader::LoadEntity(IPersistableEntity &entity)
{
	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot load entity because the database is not open");

	std::string keyPath = GetKeyPath(entity.GetKey(), hierarchy_.GetSerializationStructure());
	if (keyPath.empty())
		return;

	LoadWithParentKey(entity, GetParentKeyPath(keyPath));
}

void EntityLoader::LoadWithParentKey(IPersistableEntity &entity, const std::string_view parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : std::string(parentKey) + "." + entity.GetKey();

	boost::optional<pt::ptree &> tree = hierarchy_.GetSerializationStructure().get_child_optional(searchPath);
	if (!tree)
		throw std::runtime_error("Cannot locate entity key in the loading json structure");

	entity.Load(*tree, GetDatabase());

	for (const std::pair<std::string, pt::ptree> &child : *tree)
	{
		auto& [key, _] = child;
		if (!registry_.HasRegisteredKey(key))
			continue;

		std::unique_ptr<IPersistableEntity> memberEntity = registry_.GenerateEntity(key);
		entity.AggregateMember<Entity>(std::move(memberEntity));

		auto childEntity = std::dynamic_pointer_cast<IPersistableEntity>(entity.GetAggregatedMember(key));
		LoadWithParentKey(*childEntity, searchPath);
	}
}
