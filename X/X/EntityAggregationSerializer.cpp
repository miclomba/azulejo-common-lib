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

std::string EntityAggregationSerializer::GetKeyPath(const std::string& key, const Entity& entity) const
{
	Entity::Members members = entity.GetAggregatedMembers();

	if (entity.GetKey() == key)
		return entity.GetKey();
	else
	{
		for (auto& member : members)
		{
			std::string memKey = member.first;
			std::shared_ptr<Entity> memObj = member.second;

			std::string returnedKey = GetKeyPath(key, *memObj.get());
			size_t pos = returnedKey.find_last_of('.');
			if ((pos == std::string::npos && returnedKey == key) || (pos != std::string::npos && returnedKey.substr(pos + 1) == key))
				return memKey + "." + returnedKey;
		}
	}
	return "";
}

void EntityAggregationSerializer::Serialize(const Entity& entity)
{
	std::string keyPath = GetKeyPath(entity.GetKey(), entity);
	if (keyPath.empty())
		throw std::runtime_error("Cannot serialize entity because key=" + entity.GetKey() + " is not present in the loaded serialization structure");

	std::string parentKey;

	size_t pos = keyPath.find_last_of('.');
	if (pos != std::string::npos)
		parentKey = keyPath.substr(0, pos);

	SerializeWithParentKey(entity, parentKey);
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
