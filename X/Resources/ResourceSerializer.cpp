#include "ResourceSerializer.h"

#include <stdexcept>
#include <string>

#include "Resource.h"

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
	if (serializationPath_.empty())
		throw std::runtime_error("No serialization path set for the ResourceSerializer");

	return serializationPath_.string();
}

} // end namespace resource

