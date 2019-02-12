#ifndef resource_resourceserializer_h
#define resource_resourceserializer_h

#include <filesystem>
#include <fstream>
#include <string>

#include <boost/property_tree/ptree.hpp>

#include "config.h"
#include "IResource.h"

namespace resource {

class RESOURCE_DLL_EXPORT ResourceSerializer
{
public:
	virtual ~ResourceSerializer();

	static ResourceSerializer* GetInstance();
	static void ResetInstance();

	template<typename T>
	void Serialize(const std::string& key, const IResource<T>& resource);

	void SetSerializationPath(const std::string& binaryFilePath);
	std::string GetSerializationPath() const;

	void SetSerializationStructure(boost::property_tree::ptree* const tree);
	boost::property_tree::ptree* GetSerializationStructure() const;

private:
	ResourceSerializer();
	ResourceSerializer(const ResourceSerializer&) = delete;
	ResourceSerializer& operator=(const ResourceSerializer&) = delete;
	ResourceSerializer(ResourceSerializer&&) = delete;
	ResourceSerializer& operator=(ResourceSerializer&&) = delete;

	static ResourceSerializer* instance_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree* serializationStructure_{nullptr};
};

#include "ResourceSerializer.hpp"

} // end namespace resource

#endif // resource_resourceserializer_h

