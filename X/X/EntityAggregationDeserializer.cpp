#include "EntityAggregationDeserializer.h"

#include <fstream>
#include <memory>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Entity.h"

namespace global {

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
		boost::property_tree::read_json(file, serializationStructure_);
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

void EntityAggregationDeserializer::Deserialize(Entity& entity)
{
	DeserializeWithParentKey(entity);
}

void EntityAggregationDeserializer::DeserializeWithParentKey(Entity& entity, const std::string& parentKey)
{
	if (!HasSerializationStructure())
		throw std::runtime_error("Cannot deserialize entity because no serialization structure has been loaded");

	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();

	auto tree = serializationStructure_.get_child_optional(searchPath);
	if (!tree)
	{
		throw std::runtime_error("Cannot locate entity identifier in the deserialization json structure");
	}
	else
	{
		std::string relativePath = searchPath;
		std::replace(relativePath.begin(), relativePath.end(), '.', '/');

		std::string absolutePath = (serializationPath_.parent_path() / relativePath).string();

		entity.Load(*tree, absolutePath);
	}

	for (auto& child : *tree)
	{
		std::string key = child.first;
		auto memberEntity = GenerateEntity(key);
		entity.AggregateMember(std::move(memberEntity));

		DeserializeWithParentKey(entity.GetAggregatedMember(key), searchPath);
	}
}

std::unique_ptr<Entity> EntityAggregationDeserializer::GenerateEntity(const std::string& key)
{
	if (keyToEntityMap_.find(key) == keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " is not registered with the EntityAggregationDeserializer");

	std::unique_ptr<Entity> entity = keyToEntityMap_[key]();
	entity->SetKey(key);

	return std::move(entity);
}

} // end namespace global
