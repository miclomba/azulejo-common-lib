#include "EntityAggregationSerializer.h"

#include <fstream>
#include <memory>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Entity.h"

namespace fs = boost::filesystem;

using boost::property_tree::ptree;
using boost::property_tree::read_json;

using path_type = boost::property_tree::ptree::path_type;

namespace
{
bool FindKeyPath(const ptree& root, const ptree& node, path_type& path) {
    if (std::addressof(root) == std::addressof(node))
        return true;

    for (auto& child : root) {
        auto next = path;
        next /= child.first;

        if ( std::addressof(child.second) == std::addressof(node) || FindKeyPath(child.second, node, next))
        {
            path = next;
            return true;
        }
    }

    return false;
}

std::string KeyPathToFilePath(const std::string& keyPath)
{
	auto pos = keyPath.find_first_of('.');
	if (pos = std::string::npos)
		return keyPath;
	return (fs::path(keyPath.substr(0, pos)) / KeyPathToFilePath(keyPath.substr(pos + 1))).string();
}

std::string FindRelativePath(const ptree& root, const ptree& node) {
    path_type path;

    if (!FindKeyPath(root, node, path))
        throw std::range_error("Subtree path could not be found.");

    return KeyPathToFilePath(path.dump());
}
}

namespace global {

EntityAggregationSerializer* EntityAggregationSerializer::instance_{ nullptr };

EntityAggregationSerializer::EntityAggregationSerializer() {};
EntityAggregationSerializer::~EntityAggregationSerializer() {};

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
		read_json(file, serializationStructure_);
	else
		throw std::runtime_error("Could not open " + pathToJSON + " when loading serialization structure");
}

void EntityAggregationSerializer::Serialize(const Entity& entity)
{
	auto tree = serializationStructure_.get_child_optional(entity.GetKey());
	if (!tree)
		throw std::runtime_error("Cannot locate entity identifier in the serialization json structure");
	else
	{
		std::string relativePath = FindRelativePath(serializationStructure_, *tree);
		std::string absolutePath = (fs::path(serializationPath_) / relativePath).string();

		entity.Save(*tree, absolutePath);
	}

	auto& members = entity.GetAggregatedMembers();
	for (auto& m : members)
		Serialize(*m.second);
}

std::shared_ptr<Entity> EntityAggregationSerializer::Deserialize(const std::string& key)
{
	auto entity = std::make_shared<Entity>(); // = FactoryCreateEntityFromKey(key)

	auto tree = serializationStructure_.get_child_optional(key);
	if (!tree)
		throw std::runtime_error("Cannot locate entity identifier in the deserialization json structure");
	else
	{
		std::string relativePath = FindRelativePath(serializationStructure_, *tree);
		std::string absolutePath = (fs::path(serializationPath_) / relativePath).string();

		entity->Load(*tree, absolutePath);
	}

	auto& memberKeys = entity->GetAggregatedMemberKeys();
	for (auto& k : memberKeys)
		entity->AggregateMember(k,Deserialize(k));

	return entity;
}

} // end namespace global
