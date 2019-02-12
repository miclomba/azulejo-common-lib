#include "ResourceSerializer.h"

#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "IResource.h"

using boost::property_tree::ptree;

namespace resource {

ResourceSerializer* ResourceSerializer::instance_ = nullptr;

ResourceSerializer::ResourceSerializer() = default;
ResourceSerializer::~ResourceSerializer() = default;

ResourceSerializer* ResourceSerializer::GetInstance()
{
	if (!instance_)
		instance_ = new ResourceSerializer();
	return instance_;
}

void ResourceSerializer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void ResourceSerializer::SetSerializationPath(const std::string& binaryFilePath)
{
	serializationPath_ = binaryFilePath;
}

std::string ResourceSerializer::GetSerializationPath() const
{
	return serializationPath_.string();
}

void ResourceSerializer::SetSerializationStructure(ptree* const tree)
{
	serializationStructure_ = tree;
}

ptree* ResourceSerializer::GetSerializationStructure() const
{
	return serializationStructure_;
}

} // end namespace resource

