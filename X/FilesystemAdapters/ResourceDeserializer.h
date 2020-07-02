#ifndef filesystem_adapters_resourcedeserializer_h
#define filesystem_adapters_resourcedeserializer_h

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include "Config/filesystem.h"

#include "config.h"
#include "ISerializableResource.h"

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
	void RegisterResource(const std::string& key, std::function<std::unique_ptr<ISerializableResource>(void)> constructor);
	void UnregisterResource(const std::string& key);
	void UnregisterAll();
	bool HasSerializationKey(const std::string& key) const;

	std::unique_ptr<ISerializableResource> Deserialize(const std::string& key);
	std::unique_ptr<ISerializableResource> GenerateResource(const std::string& key) const;
private:
	ResourceDeserializer();
	ResourceDeserializer(const ResourceDeserializer&) = delete;
	ResourceDeserializer& operator=(const ResourceDeserializer&) = delete;
	ResourceDeserializer(ResourceDeserializer&&) = delete;
	ResourceDeserializer& operator=(ResourceDeserializer&&) = delete;

	std::string GetResourceExtension() const;

	static ResourceDeserializer* instance_;
	Path serializationPath_;
	mutable std::map<std::string, std::function<std::unique_ptr<ISerializableResource>(void)>> keyToResourceMap_;
};

#include "ResourceDeserializer.hpp"
} // end namespace filesystem_adapters

#endif // filesystem_adapters_resourcedeserializer_h