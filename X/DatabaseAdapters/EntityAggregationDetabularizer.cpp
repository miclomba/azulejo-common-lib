#include "EntityAggregationDetabularizer.h"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Entities/Entity.h"
#include "ITabularizableEntity.h"
#include "Sqlite.h"

namespace pt = boost::property_tree;

using database_adapters::EntityAggregationDetabularizer;
using database_adapters::ITabularizableEntity;
using database_adapters::Sqlite;
using entity::Entity;

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

EntityAggregationDetabularizer* EntityAggregationDetabularizer::instance_ = nullptr;

EntityAggregationDetabularizer::EntityAggregationDetabularizer() = default;
EntityAggregationDetabularizer::~EntityAggregationDetabularizer()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

EntityAggregationDetabularizer* EntityAggregationDetabularizer::GetInstance()
{
	if (!instance_)
		instance_ = new EntityAggregationDetabularizer();
	return instance_;
}

void EntityAggregationDetabularizer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

entity::EntityRegistry<ITabularizableEntity>& EntityAggregationDetabularizer::GetRegistry()
{
	return registry_;
}

void EntityAggregationDetabularizer::LoadSerializationStructure(const std::string& pathToJSON)
{
	serializationPath_ = pathToJSON;

	std::ifstream file(pathToJSON);
	if (file)
	{
		boost::property_tree::read_json(file, serializationStructure_);
	}
	else
	{
		serializationPath_.clear();
		throw std::runtime_error("Could not open " + pathToJSON + " when loading tabularization structure");
	}
}

bool EntityAggregationDetabularizer::HasSerializationStructure() const
{
	return !serializationPath_.empty() && !serializationStructure_.empty();
}

pt::ptree EntityAggregationDetabularizer::GetSerializationStructure() const
{
	if (HasSerializationStructure())
		return serializationStructure_;
	throw std::runtime_error("EntityAggregationDetabularizer has no tabularization structure");
}

void EntityAggregationDetabularizer::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

void EntityAggregationDetabularizer::OpenDatabase(const std::filesystem::path& dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("EntityAggregationDetabularizer already has a database set");
	databaseAdapter_.Open(dbPath);
}

Sqlite& EntityAggregationDetabularizer::GetDatabase()
{
	return databaseAdapter_;
}

void EntityAggregationDetabularizer::LoadEntity(ITabularizableEntity& entity)
{
	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot detabularize entity because the database is not open");

	if (!HasSerializationStructure())
		return;

	std::string keyPath = GetKeyPath(entity.GetKey(), serializationStructure_);
	if (keyPath.empty())
		return;

	LoadWithParentKey(entity, GetParentKeyPath(keyPath));
}

void EntityAggregationDetabularizer::LoadWithParentKey(ITabularizableEntity& entity, const Key& parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();

	boost::optional<pt::ptree&> tree = serializationStructure_.get_child_optional(searchPath);
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

		auto childEntity = std::static_pointer_cast<ITabularizableEntity>(entity.GetAggregatedMember(key));
		LoadWithParentKey(*childEntity, searchPath);
	}
}

