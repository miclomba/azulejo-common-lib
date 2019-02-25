#ifndef resource_resourceserializer_h
#define resource_resourceserializer_h

#include <filesystem>
#include <fstream>
#include <string>

#include "config.h"
#include "Resource.h"

namespace resource {

class RESOURCE_DLL_EXPORT ResourceSerializer
{
public:
	virtual ~ResourceSerializer();

	static ResourceSerializer* GetInstance();
	static void ResetInstance();

	void SetSerializationPath(const std::string& binaryFilePath);
	std::string GetSerializationPath() const;

	template<typename T>
	void Serialize(const Resource<T>& resource, const std::string& key);

private:
	ResourceSerializer();
	ResourceSerializer(const ResourceSerializer&) = delete;
	ResourceSerializer& operator=(const ResourceSerializer&) = delete;
	ResourceSerializer(ResourceSerializer&&) = delete;
	ResourceSerializer& operator=(ResourceSerializer&&) = delete;

	static ResourceSerializer* instance_;
	std::filesystem::path serializationPath_;
};

#include "ResourceSerializer.hpp"

} // end namespace resource

#endif // resource_resourceserializer_h

