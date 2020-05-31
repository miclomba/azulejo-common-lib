#include "EntityHierarchy.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;
namespace fs = std::filesystem;

using entity::EntityHierarchy;

EntityHierarchy::EntityHierarchy() = default;
EntityHierarchy::EntityHierarchy(const EntityHierarchy&) = default;
EntityHierarchy& EntityHierarchy::operator=(const EntityHierarchy&) = default;
EntityHierarchy::EntityHierarchy(EntityHierarchy&&) = default;
EntityHierarchy& EntityHierarchy::operator=(EntityHierarchy&&) = default;
EntityHierarchy::~EntityHierarchy() = default;

void EntityHierarchy::LoadSerializationStructure(const fs::path& pathToJSON)
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
		throw std::runtime_error("Could not open " + pathToJSON.string() + " when loading serialization structure");
	}
}

bool EntityHierarchy::HasSerializationStructure() const
{
	return !serializationPath_.empty() || !serializationStructure_.empty();
}

pt::ptree& EntityHierarchy::GetSerializationStructure()
{
	return serializationStructure_;
}

std::filesystem::path EntityHierarchy::GetSerializationPath() const
{
	return serializationPath_;
}

void EntityHierarchy::SetSerializationPath(const fs::path& pathToJSON)
{
	serializationPath_ = pathToJSON;
}

