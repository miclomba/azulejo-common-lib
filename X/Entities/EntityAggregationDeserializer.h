#ifndef entity_entityaggregationdeserializer_h
#define entity_entityaggregationdeserializer_h

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "ISerializableEntity.h"

namespace entity {

class ENTITY_DLL_EXPORT EntityAggregationDeserializer
{
public:
	virtual ~EntityAggregationDeserializer();

	static EntityAggregationDeserializer* GetInstance();
	static void ResetInstance();

	void LoadSerializationStructure(const std::string& pathToJSON);
	bool HasSerializationStructure() const;
	bool HasSerializationKey(const std::string& key) const;

	void Deserialize(ISerializableEntity& entity);

	template<typename T>
	void RegisterEntity(const std::string& key);
	void UnregisterEntity(const std::string& key);

	std::unique_ptr<ISerializableEntity> GenerateEntity(const std::string& key) const;

private:
	EntityAggregationDeserializer();
	EntityAggregationDeserializer(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer& operator=(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer(EntityAggregationDeserializer&&) = delete;
	EntityAggregationDeserializer& operator=(EntityAggregationDeserializer&&) = delete;

	void DeserializeWithParentKey(ISerializableEntity& entity, const std::string& parentKey = "");

	static EntityAggregationDeserializer* instance_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;
	mutable std::map<std::string, std::function<std::unique_ptr<ISerializableEntity>(void)>> keyToEntityMap_;
};

#include "EntityAggregationDeserializer.hpp"

} // end namespace entity
#endif // entity_entityaggregationdeserializer_h

