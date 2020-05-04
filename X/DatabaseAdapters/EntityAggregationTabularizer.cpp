#include "EntityAggregationTabularizer.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "ITabularizableEntity.h"
#include "Sqlite.h"

namespace pt = boost::property_tree;

using database_adapters::EntityAggregationTabularizer;
using database_adapters::ITabularizableEntity;
using database_adapters::Sqlite;
using entity::Entity;
using entity::EntityHierarchy;

using Key = Entity::Key;

EntityAggregationTabularizer* EntityAggregationTabularizer::instance_ = nullptr;

EntityAggregationTabularizer::EntityAggregationTabularizer() = default;

EntityAggregationTabularizer::~EntityAggregationTabularizer()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

EntityAggregationTabularizer* EntityAggregationTabularizer::GetInstance()
{
	if (!instance_)
		instance_ = new EntityAggregationTabularizer();
	return instance_;
}

void EntityAggregationTabularizer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void EntityAggregationTabularizer::Tabularize(const ITabularizableEntity& entity)
{
	if (!databaseAdapter_.IsOpen())
		throw std::runtime_error("Cannot tabularize entity because the database is not open");
	if (entity.GetKey().empty())
		throw std::runtime_error("Cannot tabularize entity because key=" + entity.GetKey() + " is not present in the loaded tabularization structure");
	TabularizeWithParentKey(entity, "");
}

void EntityAggregationTabularizer::TabularizeWithParentKey(const ITabularizableEntity& entity, const Key& parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();
	
	hierarchy_.GetSerializationStructure().put_child(searchPath, pt::ptree());

	entity.Save(GetDatabase());

	std::vector<std::string> keys = entity.GetAggregatedMemberKeys<ITabularizableEntity>();
	for (const std::string& key : keys)
	{
		Entity::SharedEntity& member = entity.GetAggregatedMember(key);
		auto memberPtr = static_cast<ITabularizableEntity*>(member.get());
		TabularizeWithParentKey(*memberPtr,searchPath);
	}

	if (parentKey.empty())
		boost::property_tree::json_parser::write_json(hierarchy_.GetSerializationPath().string(), hierarchy_.GetSerializationStructure());
}

void EntityAggregationTabularizer::OpenDatabase(const std::filesystem::path& dbPath)
{
	if (databaseAdapter_.IsOpen())
		throw std::runtime_error("EntityAggregationDetabularizer already has a database open");
	databaseAdapter_.Open(dbPath);
}

void EntityAggregationTabularizer::CloseDatabase()
{
	if (databaseAdapter_.IsOpen())
		databaseAdapter_.Close();
}

Sqlite& EntityAggregationTabularizer::GetDatabase()
{
	return databaseAdapter_;
}

EntityHierarchy& EntityAggregationTabularizer::GetHierarchy()
{
	return hierarchy_;
}

