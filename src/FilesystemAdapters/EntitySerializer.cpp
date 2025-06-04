#include "FilesystemAdapters/EntitySerializer.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Entities/Entity.h"
#include "Entities/EntityHierarchy.h"
#include "FilesystemAdapters/ISerializableEntity.h"

namespace pt = boost::property_tree;

using entity::Entity;
using entity::EntityHierarchy;
using filesystem_adapters::EntitySerializer;
using filesystem_adapters::ISerializableEntity;

using Key = Entity::Key;

EntitySerializer *EntitySerializer::instance_ = nullptr;

EntitySerializer::EntitySerializer() = default;
EntitySerializer::~EntitySerializer() = default;

EntitySerializer *EntitySerializer::GetInstance()
{
	if (!instance_)
		instance_ = new EntitySerializer();
	return instance_;
}

void EntitySerializer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void EntitySerializer::Serialize(const ISerializableEntity &entity)
{
	if (entity.GetKey().empty())
		throw std::runtime_error("Cannot serialize entity because key=" + entity.GetKey() + " is not present in the loaded serialization structure");
	SerializeWithParentKey(entity, "");
}

void EntitySerializer::SerializeWithParentKey(const ISerializableEntity &entity, const Key &parentKey)
{
	std::string searchPath = parentKey.empty() ? entity.GetKey() : parentKey + "." + entity.GetKey();

	pt::ptree &tree = hierarchy_.GetSerializationStructure().put_child(searchPath, pt::ptree());

	std::string relativePath = searchPath;
	std::replace(relativePath.begin(), relativePath.end(), '.', '/');

	std::string absolutePath = (hierarchy_.GetSerializationPath().parent_path() / relativePath).string();

	entity.Save(tree, absolutePath);

	std::vector<std::string> keys = entity.GetAggregatedMemberKeys<ISerializableEntity>();
	for (const std::string &key : keys)
	{
		Entity::SharedEntity &member = entity.GetAggregatedMember(key);
		auto memberPtr = dynamic_cast<ISerializableEntity *>(member.get());
		SerializeWithParentKey(*memberPtr, searchPath);
	}

	if (parentKey.empty())
		boost::property_tree::json_parser::write_json(hierarchy_.GetSerializationPath().string(), hierarchy_.GetSerializationStructure());
}

EntityHierarchy &EntitySerializer::GetHierarchy()
{
	return hierarchy_;
}
