#include "EntityAggregationSerializer.h"

#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Entity.h"

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

void EntityAggregationSerializer::Serialize(const Entity& entity)
{
	// find parent key path here and pass to this functions
	SerializeWithParentKey(entity);
}

void EntityAggregationSerializer::SerializeWithParentKey(const Entity& entity, const std::string& parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();
	
	auto tree = serializationStructure_.put_child(searchPath, ptree());

	std::string relativePath = searchPath;
	std::replace(relativePath.begin(), relativePath.end(), '.', '/');

	std::string absolutePath = (serializationPath_.parent_path() / relativePath).string();

	entity.Save(tree, absolutePath);

	auto& members = entity.GetAggregatedMembers();
	for (auto& m : members)
		SerializeWithParentKey(*m.second,searchPath);

	if (parentKey.empty())
		boost::property_tree::json_parser::write_json(serializationPath_.string(), serializationStructure_);
}

void EntityAggregationSerializer::SetSerializationPath(const std::string& pathToJSON)
{
	serializationPath_ = pathToJSON;
}

std::string EntityAggregationSerializer::GetSerializationPath() const
{
	return serializationPath_.string();
}

} // end namespace global
