#include "EntityAggregationDeserializer.h"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ISerializableEntity.h"

namespace pt = boost::property_tree;

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

namespace entity {

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

void EntityAggregationDeserializer::LoadSerializationStructure(const std::string& pathToJSON)
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
		throw std::runtime_error("Could not open " + pathToJSON + " when loading serialization structure");
	}
}

bool EntityAggregationDeserializer::HasSerializationStructure() const
{
	return !serializationPath_.empty() && !serializationStructure_.empty();
}

bool EntityAggregationDeserializer::HasSerializationKey(const Key& key) const
{
	return keyToEntityMap_.find(key) != keyToEntityMap_.cend();
}

void EntityAggregationDeserializer::UnregisterEntity(const Key& key)
{
	if (keyToEntityMap_.find(key) == keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " not already registered with the EntityAggregationDeserializer");

	keyToEntityMap_.erase(key);
}

std::unique_ptr<ISerializableEntity> EntityAggregationDeserializer::GenerateEntity(const Key& key) const
{
	if (!HasSerializationKey(key))
		throw std::runtime_error("Key=" + key + " is not registered with the EntityAggregationDeserializer");

	std::unique_ptr<ISerializableEntity> entity = keyToEntityMap_[key]();
	entity->SetKey(key);

	return std::move(entity);
}

void EntityAggregationDeserializer::Deserialize(ISerializableEntity& entity)
{
	if (!HasSerializationStructure())
		return;

	std::string keyPath = GetKeyPath(entity.GetKey(), serializationStructure_);
	if (keyPath.empty())
		return;

	DeserializeWithParentKey(entity, GetParentKeyPath(keyPath));
}

void EntityAggregationDeserializer::DeserializeWithParentKey(ISerializableEntity& entity, const Key& parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();

	boost::optional<pt::ptree&> tree = serializationStructure_.get_child_optional(searchPath);
	if (!tree)
	{
		throw std::runtime_error("Cannot locate entity key in the deserialization json structure");
	}
	else
	{
		std::string relativePath = searchPath;
		std::replace(relativePath.begin(), relativePath.end(), '.', '/');

		std::string absolutePath = (serializationPath_.parent_path() / relativePath).string();

		entity.Load(*tree, absolutePath);
	}

	for (const std::pair<std::string, pt::ptree>& child : *tree)
	{
		std::string key = child.first;
		if (!HasSerializationKey(key))
			continue;

		std::unique_ptr<ISerializableEntity> memberEntity = GenerateEntity(key);
		entity.AggregateMember<Entity>(std::move(memberEntity));

		auto childEntity = std::static_pointer_cast<ISerializableEntity>(entity.GetAggregatedMember(key));
		DeserializeWithParentKey(*childEntity, searchPath);
	}
}

} // end namespace entity
