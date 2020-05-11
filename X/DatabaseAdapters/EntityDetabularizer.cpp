#include "EntityDetabularizer.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Entities/Entity.h"
#include "Entities/EntityHierarchy.h"
#include "Entities/EntityRegistry.h"
#include "ITabularizableEntity.h"
#include "Sqlite.h"

namespace pt = boost::property_tree;

using database_adapters::EntityDetabularizer;
using database_adapters::ITabularizableEntity;
using database_adapters::Sqlite;
using entity::Entity;
using entity::EntityHierarchy;
using entity::EntityRegistry;

using Key = Entity::Key;

namespace
{
std::string GetKeyPath(const Key& key, const pt::ptree& tree)
{
	for (const std::pair<std::string, pt::ptree>& keyValue : tree)
	{
		std::string nodeKey = keyValue.first;
		pt::ptree node = keyValue.second;

		if (nodeKey == key)
		{
			return nodeKey;
		}
		else
		{
			std::string returnedKey = GetKeyPath(key, node);
			size_t pos = returnedKey.find_last_of('.');
			if ((pos == std::string::npos && returnedKey == key) || (pos != std::string::npos && returnedKey.substr(pos+1) == key))
				return nodeKey + "." + returnedKey;
		}
	}
	return "";
}

std::string GetParentKeyPath(const std::string& keyPath)
{
	std::string parentKeyPath;

	size_t pos = keyPath.find_last_of('.');
	if (pos != std::string::npos)
		parentKeyPath = keyPath.substr(0, pos);

	return parentKeyPath;
}
} // end namespace anonymous

EntityDetabularizer* EntityDetabularizer::instance_ = nullptr;

EntityDetabularizer::EntityDetabularizer() = default;
EntityDetabularizer::~EntityDetabularizer()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

EntityDetabularizer* EntityDetabularizer::GetInstance()
{
	if (!instance_)
		instance_ = new EntityDetabularizer();
	return instance_;
}

void EntityDetabularizer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

EntityRegistry<ITabularizableEntity>& EntityDetabularizer::GetRegistry()
{
	return registry_;
}

EntityHierarchy& EntityDetabularizer::GetHierarchy()
{
	return hierarchy_;
}

void EntityDetabularizer::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

void EntityDetabularizer::OpenDatabase(const std::filesystem::path& dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("EntityDetabularizer already has a database set");
	databaseAdapter_.Open(dbPath);
}

Sqlite& EntityDetabularizer::GetDatabase()
{
	return databaseAdapter_;
}

void EntityDetabularizer::LoadEntity(ITabularizableEntity& entity)
{
	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot detabularize entity because the database is not open");

	if (!hierarchy_.HasSerializationStructure())
		return;

	std::string keyPath = GetKeyPath(entity.GetKey(), hierarchy_.GetSerializationStructure());
	if (keyPath.empty())
		return;

	LoadWithParentKey(entity, GetParentKeyPath(keyPath));
}

void EntityDetabularizer::LoadWithParentKey(ITabularizableEntity& entity, const Key& parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();

	boost::optional<pt::ptree&> tree = hierarchy_.GetSerializationStructure().get_child_optional(searchPath);
	if (!tree)
		throw std::runtime_error("Cannot locate entity key in the detabularization json structure");

	entity.Load(GetDatabase());

	for (const std::pair<std::string, pt::ptree>& child : *tree)
	{
		std::string key = child.first;
		if (!registry_.HasRegisteredKey(key))
			continue;

		std::unique_ptr<ITabularizableEntity> memberEntity = registry_.GenerateEntity(key);
		entity.AggregateMember<Entity>(std::move(memberEntity));

		auto childEntity = std::dynamic_pointer_cast<ITabularizableEntity>(entity.GetAggregatedMember(key));
		LoadWithParentKey(*childEntity, searchPath);
	}
}

