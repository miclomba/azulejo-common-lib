#include "EntityAggregationSerializer.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Entity.h"

namespace fs = std::filesystem;
using boost::property_tree::ptree;

namespace global {

EntityAggregationSerializer* EntityAggregationSerializer::instance_ = nullptr;

EntityAggregationSerializer::EntityAggregationSerializer() = default;
EntityAggregationSerializer::~EntityAggregationSerializer() = default;

EntityAggregationSerializer* EntityAggregationSerializer::GetInstance()
{
	if (!instance_)
		instance_ = new EntityAggregationSerializer();
	return instance_;
}

void EntityAggregationSerializer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void EntityAggregationSerializer::LoadSerializationStructure(const std::string& pathToJSON)
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

bool EntityAggregationSerializer::HasSerializationStructure() const
{
	return !serializationPath_.empty() && !serializationStructure_.empty();
}

void EntityAggregationSerializer::Serialize(const Entity& entity, const std::string& parentPath)
{
	if (!HasSerializationStructure())
		throw std::runtime_error("Cannot serialize entity because no serialization structure has been loaded");
	
	std::string searchPath = parentPath.empty() ? entity.GetKey() : parentPath + "." + entity.GetKey();
	
	auto tree = serializationStructure_.get_child_optional(searchPath);
	if (!tree)
	{
		throw std::runtime_error("Cannot locate entity identifier in the serialization json structure");
	}
	else
	{
		std::string relativePath = searchPath;
		std::replace(relativePath.begin(), relativePath.end(), '.', '/');

		std::string absolutePath = (serializationPath_.parent_path() / relativePath).string();

		entity.Save(*tree, absolutePath);
	}

	auto& members = entity.GetAggregatedMembers();
	for (auto& m : members)
		Serialize(*m.second,searchPath);
}

std::shared_ptr<Entity> EntityAggregationSerializer::Deserialize(const std::string& key)
{
	/*
	if (!HasSerializationStructure())
		throw std::runtime_error("Cannot deserialize entity because no serialization structure has been loaded");

	auto entity = std::make_shared<Entity>(); // = FactoryCreateEntityFromKey(key)
	std::string searchPath = key.empty() ? entity->GetKey() : key + "." + entity->GetKey();

	auto tree = serializationStructure_.get_child_optional(key);
	if (!tree)
		throw std::runtime_error("Cannot locate entity identifier in the deserialization json structure");
	else
	{
		std::string relativePath = "";// FindRelativePath(serializationStructure_, *tree);
		std::string absolutePath = (fs::path(serializationPath_) / relativePath).string();

		entity->Load(*tree, absolutePath);
	}

	auto& memberKeys = entity->GetAggregatedMemberKeys();
	for (auto& k : memberKeys)
		entity->AggregateMember(Deserialize(k));

	return entity;
	*/
	throw std::runtime_error("not implemented");
}

} // end namespace global
