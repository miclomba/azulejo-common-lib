#ifndef filesystem_adapters_resourcedeserializer_h
#define filesystem_adapters_resourcedeserializer_h

#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include "config.h"
#include "Resources/IResource.h"

namespace filesystem_adapters {

class FILESYSTEM_ADAPTERS_DLL_EXPORT ResourceDeserializer
{
public:
	virtual ~ResourceDeserializer();

	static ResourceDeserializer* GetInstance();
	static void ResetInstance();

	void SetSerializationPath(const std::string& binaryFilePath);
	std::string GetSerializationPath() const;

	template<typename T>
	void RegisterResource(const std::string& key, std::function<std::unique_ptr<resource::IResource>(void)> constructor);
	void UnregisterResource(const std::string& key);
	void UnregisterAll();
	bool HasSerializationKey(const std::string& key) const;

	std::unique_ptr<resource::IResource> Deserialize(const std::string& key);
	std::unique_ptr<resource::IResource> GenerateResource(const std::string& key) const;
private:
	ResourceDeserializer();
	ResourceDeserializer(const ResourceDeserializer&) = delete;
	ResourceDeserializer& operator=(const ResourceDeserializer&) = delete;
	ResourceDeserializer(ResourceDeserializer&&) = delete;
	ResourceDeserializer& operator=(ResourceDeserializer&&) = delete;

	std::string GetResourceExtension() const;

	static ResourceDeserializer* instance_;
	std::filesystem::path serializationPath_;
	mutable std::map<std::string, std::function<std::unique_ptr<resource::IResource>(void)>> keyToResourceMap_;
};

#include "ResourceDeserializer.hpp"
} // end namespace filesystem_adapters

#endif // filesystem_adapters_resourcedeserializer_h