#include "ResourceDeserializer.h"
/*
#include <fstream>
#include <memory>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "IResource.h"

using boost::property_tree::ptree;

namespace
{
std::string GetKeyPath(const std::string& key, const ptree& tree)
{
	for (auto& keyValue : tree)
	{
		std::string nodeKey = keyValue.first;
		ptree node = keyValue.second;

		if (nodeKey == key)
			return nodeKey;
		else
		{
			std::string returnedKey = GetKeyPath(key, node);
			size_t pos = returnedKey.find_last_of('.');
			if ((pos == std::string::npos && returnedKey == key) || (pos != std::string::npos && returnedKey.substr(pos+1) == key))
				return nodeKey + "." + returnedKey;
		}
	}
	return "";
}
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

void ResourceDeserializer::LoadSerializationStructure(const std::string& pathToJSON)
{
	serializationPath_ = pathToJSON;

	std::ifstream file(pathToJSON);
	if (file)
		boost::property_tree::read_json(file, serializationStructure_);
	else
	{
		serializationPath_.clear();
		throw std::runtime_error("Could not open " + pathToJSON + " when loading serialization structure");
	}
}

bool ResourceDeserializer::HasSerializationStructure() const
{
	return !serializationPath_.empty() && !serializationStructure_.empty();
}

bool ResourceDeserializer::HasSerializationKey(const std::string& key) const
{
	return !GetKeyPath(key, serializationStructure_).empty() ? true : false;
}

void ResourceDeserializer::UnregisterEntity(const std::string& key)
{
	if (keyToEntityMap_.find(key) == keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " not already registered with the ResourceDeserializer");

	keyToEntityMap_.erase(key);
}

template<typename T>
void ResourceDeserializer::Deserialize(IResource<T>& resource)
{
	if (!HasSerializationStructure())
		throw std::runtime_error("Cannot deserialize resource because no serialization structure has been loaded");

	std::string keyPath = GetKeyPath(resource.GetKey(), serializationStructure_);
	if (keyPath.empty())
		throw std::runtime_error("Cannot deserialize resource because key=" + resource.GetKey() + " is not present in the loaded serialization structure");

	std::string parentKey;

	size_t pos = keyPath.find_last_of('.');
	if (pos != std::string::npos)
		parentKey = keyPath.substr(0,pos);

	DeserializeWithParentKey(resource, parentKey);
}

template<typename T>
void ResourceDeserializer::DeserializeWithParentKey(IResource<T>& resource, const std::string& parentKey)
{
	if (!HasSerializationStructure())
		throw std::runtime_error("Cannot deserialize resource because no serialization structure has been loaded");

	std::string searchPath = parentKey.empty() ? resource.GetKey() : parentKey + "." + resource.GetKey();

	auto tree = serializationStructure_.get_child_optional(searchPath);
	if (!tree)
	{
		throw std::runtime_error("Cannot locate resource key in the deserialization json structure");
	}
	else
	{
		std::string relativePath = searchPath;
		std::replace(relativePath.begin(), relativePath.end(), '.', '/');

		std::string absolutePath = (serializationPath_.parent_path() / relativePath).string();

		resource.Load(*tree, absolutePath);
	}

	for (auto& child : *tree)
	{
		std::string key = child.first;
		auto memberEntity = GenerateEntity(key);
		resource.AggregateMember(std::move(memberEntity));

		auto childEntity = std::static_pointer_cast<IResource<T>>(resource.GetAggregatedMember(key));
		DeserializeWithParentKey(*childEntity, searchPath);
	}
}

template<typename T>
std::unique_ptr<IResource<T>> ResourceDeserializer::GenerateEntity(const std::string& key) const
{
	if (keyToEntityMap_.find(key) == keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " is not registered with the ResourceDeserializer");

	std::unique_ptr<IResource<T>> resource = keyToEntityMap_[key]();
	resource->SetKey(key);

	return std::move(resource);
}

} // end namespace resource
*/