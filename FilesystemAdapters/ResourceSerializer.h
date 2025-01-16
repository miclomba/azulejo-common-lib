#ifndef filesystem_adapters_resourceserializer_h
#define filesystem_adapters_resourceserializer_h

#include <fstream>
#include <stdexcept>
#include <string>
#include "Config/filesystem.h"

#include "config.h"
#include "ISerializableResource.h"

namespace filesystem_adapters {

class FILESYSTEM_ADAPTERS_DLL_EXPORT ResourceSerializer
{
public:
	virtual ~ResourceSerializer();

	static ResourceSerializer* GetInstance();
	static void ResetInstance();

	void SetSerializationPath(const std::string& binaryFilePath);
	std::string GetSerializationPath() const;

	void Serialize(const ISerializableResource& resource, const std::string& key);
	void Unserialize(const std::string& key);

private:
	ResourceSerializer();
	ResourceSerializer(const ResourceSerializer&) = delete;
	ResourceSerializer& operator=(const ResourceSerializer&) = delete;
	ResourceSerializer(ResourceSerializer&&) = delete;
	ResourceSerializer& operator=(ResourceSerializer&&) = delete;

	static ResourceSerializer* instance_;
	Path serializationPath_;
};

} // end namespace filesystem_adapters

#endif // filesystem_adapters_resourceserializer_h

