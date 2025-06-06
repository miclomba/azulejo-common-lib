#include "FilesystemAdapters/ResourceDeserializer.h"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include "Config/filesystem.hpp"

#include "FilesystemAdapters/ISerializableResource.h"

namespace
{
	const std::string RESOURCE_EXT = ".bin";
} // end namespace anonymous

using filesystem_adapters::ISerializableResource;
using filesystem_adapters::ResourceDeserializer;

ResourceDeserializer *ResourceDeserializer::instance_ = nullptr;

ResourceDeserializer::ResourceDeserializer() = default;
ResourceDeserializer::~ResourceDeserializer() = default;

ResourceDeserializer *ResourceDeserializer::GetInstance()
{
	if (!instance_)
		instance_ = new ResourceDeserializer();
	return instance_;
}

void ResourceDeserializer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void ResourceDeserializer::SetSerializationPath(const std::string &binaryFilePath)
{
	serializationPath_ = binaryFilePath;
}

std::string ResourceDeserializer::GetSerializationPath() const
{
	if (serializationPath_.empty())
		throw std::runtime_error("No serialization path set for the ResourceDeserializer");
	return serializationPath_.string();
}

void ResourceDeserializer::UnregisterResource(const std::string &key)
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when unregistering resource with ResourceDeserializer");
	if (keyToResourceMap_.find(key) == keyToResourceMap_.cend())
		throw std::runtime_error("Key=" + key + " not already registered with the ResourceDeserializer");

	keyToResourceMap_.erase(key);
}

bool ResourceDeserializer::HasSerializationKey(const std::string &key) const
{
	return keyToResourceMap_.find(key) != keyToResourceMap_.cend();
}

void ResourceDeserializer::UnregisterAll()
{
	keyToResourceMap_.clear();
}

std::unique_ptr<ISerializableResource> ResourceDeserializer::Deserialize(const std::string &key)
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when deserializing resource with ResourceDeserializer");

	Path serializationPath = GetSerializationPath();
	if (serializationPath.empty())
		throw std::runtime_error("Serialization path is empty when deserializaing resource with ResourceDeserializer");

	std::string fileName = key + GetResourceExtension();
	std::ifstream inFile((serializationPath / fileName).string(), std::ios::binary);
	if (!inFile)
		throw std::runtime_error("Could not open input file: " + (serializationPath / fileName).string());

	inFile.seekg(0, std::ios::end);
	const int size = inFile.tellg();
	inFile.seekg(0, std::ios::beg);

	size_t colSizeOffset = 0;
	size_t rowSizeOffset = sizeof(size_t);
	size_t dataOffset = 2 * sizeof(size_t);

	auto buff = std::make_unique<char *>(new char[size]);

	inFile.read(*buff + colSizeOffset, sizeof(size_t));
	inFile.read(*buff + rowSizeOffset, sizeof(size_t));
	if (size > dataOffset)
		inFile.read(*buff + dataOffset, size - dataOffset);

	std::unique_ptr<ISerializableResource> arithmeticContainer = GenerateResource(key);

	arithmeticContainer->SetColumnSize(*(*buff + colSizeOffset));
	arithmeticContainer->SetRowSize(*(*buff + rowSizeOffset));
	if (size > dataOffset)
		arithmeticContainer->Assign(*buff + dataOffset, size - dataOffset);

	return arithmeticContainer;
}

std::unique_ptr<ISerializableResource> ResourceDeserializer::GenerateResource(const std::string &key) const
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when generating resource with ResourceDeserializer");
	if (keyToResourceMap_.find(key) == keyToResourceMap_.cend())
		throw std::runtime_error("Key=" + key + " is not registered with the ResourceDeserializer");

	std::unique_ptr<ISerializableResource> resource = keyToResourceMap_[key]();

	return resource;
}

std::string ResourceDeserializer::GetResourceExtension() const
{
	return ".bin";
}
