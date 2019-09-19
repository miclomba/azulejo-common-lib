#include "ResourceDeserializer.h"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

#include "Resource.h"

namespace
{
const std::string RESOURCE_EXT = ".bin";
} // end namespace anonymous

namespace resource {

ResourceDeserializer* ResourceDeserializer::instance_ = nullptr;

ResourceDeserializer::ResourceDeserializer() = default;
ResourceDeserializer::~ResourceDeserializer() = default;

ResourceDeserializer* ResourceDeserializer::GetInstance()
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

void ResourceDeserializer::SetSerializationPath(const std::string& binaryFilePath)
{
	serializationPath_ = binaryFilePath;
}

std::string ResourceDeserializer::GetSerializationPath() const
{
	if (serializationPath_.empty())
		throw std::runtime_error("No serialization path set for the ResourceDeserializer");
	return serializationPath_.string();
}

void ResourceDeserializer::UnregisterResource(const std::string& key)
{
	if (keyToResourceMap_.find(key) == keyToResourceMap_.cend())
		throw std::runtime_error("Key=" + key + " not already registered with the ResourceDeserializer");

	keyToResourceMap_.erase(key);
}

std::unique_ptr<IResource> ResourceDeserializer::Deserialize(const std::string& key)
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when deserializing resource with ResourceDeserializer");

	std::filesystem::path serializationPath = GetSerializationPath();
	if (serializationPath.empty())
		throw std::runtime_error("Serialization path is empty when deserializaing resource with ResourceDeserializer");

	std::string fileName = key + GetResourceExtension();
	std::ifstream inFile(serializationPath / fileName, std::ios::binary);
	if (!inFile)
		throw std::runtime_error("Could not open input file: " + (serializationPath / fileName).string());

	inFile.seekg(0, std::ios::end);
	const int size = inFile.tellg();
	inFile.seekg(0, std::ios::beg);

	auto buff = std::make_unique<char*>(new char[size]);
	inFile.read(*buff, size);

	std::unique_ptr<IResource> arithmeticContainer = GenerateResource(key);
	arithmeticContainer->Assign(*buff, size);
	
	return arithmeticContainer;
}

std::unique_ptr<IResource> ResourceDeserializer::GenerateResource(const std::string& key) const
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when generating resource with ResourceDeserializer");
	if (keyToResourceMap_.find(key) == keyToResourceMap_.cend())
		throw std::runtime_error("Key=" + key + " is not registered with the ResourceDeserializer");

	std::unique_ptr<IResource> resource = keyToResourceMap_[key]();

	return resource;
}

std::string ResourceDeserializer::GetResourceExtension() const
{
	return ".bin";
}

} // end namespace resource
