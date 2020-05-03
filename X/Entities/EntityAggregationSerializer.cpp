#include "EntityAggregationSerializer.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ISerializableEntity.h"

namespace pt = boost::property_tree;

using entity::Entity;
using entity::EntityAggregationSerializer;
using entity::ISerializableEntity;

using Key = Entity::Key;

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

void EntityAggregationSerializer::Serialize(const ISerializableEntity& entity)
{
	if (entity.GetKey().empty())
		throw std::runtime_error("Cannot serialize entity because key=" + entity.GetKey() + " is not present in the loaded serialization structure");
	SerializeWithParentKey(entity, "");
}

void EntityAggregationSerializer::SerializeWithParentKey(const ISerializableEntity& entity, const Key& parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();
	
	pt::ptree& tree = serializationStructure_.put_child(searchPath, pt::ptree());

	std::string relativePath = searchPath;
	std::replace(relativePath.begin(), relativePath.end(), '.', '/');

	std::string absolutePath = (serializationPath_.parent_path() / relativePath).string();

	entity.Save(tree, absolutePath);

	std::vector<std::string> keys = entity.GetAggregatedMemberKeys<ISerializableEntity>();
	for (const std::string& key : keys)
	{
		Entity::SharedEntity& member = entity.GetAggregatedMember(key);
		auto memberPtr = static_cast<ISerializableEntity*>(member.get());
		SerializeWithParentKey(*memberPtr,searchPath);
	}

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

