#ifndef filesystem_adapters_resourceserializer_h
#define filesystem_adapters_resourceserializer_h

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "config.h"
#include "Resources/Resource.h"

namespace filesystem_adapters {

class FILESYSTEM_ADAPTERS_DLL_EXPORT ResourceSerializer
{
public:
	virtual ~ResourceSerializer();

	static ResourceSerializer* GetInstance();
	static void ResetInstance();

	void SetSerializationPath(const std::string& binaryFilePath);
	std::string GetSerializationPath() const;

	void Serialize(const resource::IResource& resource, const std::string& key);

private:
	ResourceSerializer();
	ResourceSerializer(const ResourceSerializer&) = delete;
	ResourceSerializer& operator=(const ResourceSerializer&) = delete;
	ResourceSerializer(ResourceSerializer&&) = delete;
	ResourceSerializer& operator=(ResourceSerializer&&) = delete;

	static ResourceSerializer* instance_;
	std::filesystem::path serializationPath_;
};

} // end namespace filesystem_adapters

#endif // filesystem_adapters_resourceserializer_h

