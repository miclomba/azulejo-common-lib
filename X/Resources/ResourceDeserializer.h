#ifndef resource_resourcedeserializer_h
#define resource_resourcedeserializer_h
/*
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include <boost/property_tree/ptree.hpp>

#include "config.h"
#include "IResource.h"

namespace resource {

class RESOURCE_DLL_EXPORT ResourceDeserializer
{
public:
	virtual ~ResourceDeserializer();

	static ResourceDeserializer* GetInstance();
	static void ResetInstance();

	void LoadSerializationStructure(const std::string& pathToJSON);
	bool HasSerializationStructure() const;
	bool HasSerializationKey(const std::string& key) const;

	template<typename T>
	void Deserialize(IResource<T>& resource);

	template<typename T>
	void RegisterEntity(const std::string& key);
	void UnregisterEntity(const std::string& key);

	template<typename T>
	std::unique_ptr<IResource<T>> GenerateEntity(const std::string& key) const;

private:
	ResourceDeserializer();
	ResourceDeserializer(const ResourceDeserializer&) = delete;
	ResourceDeserializer& operator=(const ResourceDeserializer&) = delete;
	ResourceDeserializer(ResourceDeserializer&&) = delete;
	ResourceDeserializer& operator=(ResourceDeserializer&&) = delete;

	template<typename T>
	void DeserializeWithParentKey(IResource<T>& resource, const std::string& parentKey = "");

	static ResourceDeserializer* instance_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;

	template<typename T>
	mutable std::map<std::string, std::function<std::unique_ptr<IResource<T>>(void)>> keyToEntityMap_;
};

#include "ResourceDeserializer.hpp"

} // end namespace resource
*/
#endif // resource_resourcedeserializer_h