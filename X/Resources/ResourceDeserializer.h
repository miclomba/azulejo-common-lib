#ifndef resource_resourcedeserializer_h
#define resource_resourcedeserializer_h

#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>

#include "config.h"
#include "IResource.h"
#include "Resource.h"

namespace resource {

class RESOURCE_DLL_EXPORT ResourceDeserializer
{
public:
	virtual ~ResourceDeserializer();

	static ResourceDeserializer* GetInstance();
	static void ResetInstance();

	void SetSerializationPath(const std::string& binaryFilePath);
	std::string GetSerializationPath() const;

	std::unique_ptr<IResource> Deserialize(const std::string& key);

	template<typename T>
	void RegisterResource(const std::string& key);
	void UnregisterResource(const std::string& key);

	std::unique_ptr<IResource> GenerateResource(const std::string& key) const;
private:
	ResourceDeserializer();
	ResourceDeserializer(const ResourceDeserializer&) = delete;
	ResourceDeserializer& operator=(const ResourceDeserializer&) = delete;
	ResourceDeserializer(ResourceDeserializer&&) = delete;
	ResourceDeserializer& operator=(ResourceDeserializer&&) = delete;

	std::string GetResourceExtension() const;

	static ResourceDeserializer* instance_;
	std::filesystem::path serializationPath_;
	mutable std::map<std::string, std::function<std::unique_ptr<IResource>(void)>> keyToResourceMap_;
};

#include "ResourceDeserializer.hpp"

} // end namespace resource

#endif // resource_resourcedeserializer_h