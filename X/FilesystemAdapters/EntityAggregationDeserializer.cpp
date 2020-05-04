#include "EntityAggregationDeserializer.h"

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
#include "ISerializableEntity.h"

namespace pt = boost::property_tree;

using entity::Entity;
using entity::EntityHierarchy;
using entity::EntityRegistry;
using filesystem_adapters::EntityAggregationDeserializer;
using filesystem_adapters::ISerializableEntity;

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

EntityAggregationDeserializer* EntityAggregationDeserializer::instance_ = nullptr;

EntityAggregationDeserializer::EntityAggregationDeserializer() = default;
EntityAggregationDeserializer::~EntityAggregationDeserializer() = default;

EntityAggregationDeserializer* EntityAggregationDeserializer::GetInstance()
{
	if (!instance_)
		instance_ = new EntityAggregationDeserializer();
	return instance_;
}

void EntityAggregationDeserializer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

EntityRegistry<ISerializableEntity>& EntityAggregationDeserializer::GetRegistry()
{
	return registry_;
}

EntityHierarchy& EntityAggregationDeserializer::GetHierarchy()
{
	return hierarchy_;
}

void EntityAggregationDeserializer::LoadEntity(ISerializableEntity& entity)
{
	if (!hierarchy_.HasSerializationStructure())
		return;

	std::string keyPath = GetKeyPath(entity.GetKey(), hierarchy_.GetSerializationStructure());
	if (keyPath.empty())
		return;

	LoadWithParentKey(entity, GetParentKeyPath(keyPath));
}

void EntityAggregationDeserializer::LoadWithParentKey(ISerializableEntity& entity, const Key& parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();

	boost::optional<pt::ptree&> tree = hierarchy_.GetSerializationStructure().get_child_optional(searchPath);
	if (!tree)
		throw std::runtime_error("Cannot locate entity key in the deserialization json structure");

	std::string relativePath = searchPath;
	std::replace(relativePath.begin(), relativePath.end(), '.', '/');
	std::string absolutePath = (hierarchy_.GetSerializationPath().parent_path() / relativePath).string();
	entity.Load(*tree, absolutePath);

	for (const std::pair<std::string, pt::ptree>& child : *tree)
	{
		std::string key = child.first;
		if (!registry_.HasRegisteredKey(key))
			continue;

		std::unique_ptr<ISerializableEntity> memberEntity = registry_.GenerateEntity(key);
		entity.AggregateMember<Entity>(std::move(memberEntity));

		auto childEntity = std::static_pointer_cast<ISerializableEntity>(entity.GetAggregatedMember(key));
		LoadWithParentKey(*childEntity, searchPath);
	}
}

