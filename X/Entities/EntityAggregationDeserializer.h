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

	// structure
	void LoadSerializationStructure(const std::string& pathToJSON);
	bool HasSerializationStructure() const;
	boost::property_tree::ptree GetSerializationStructure() const;

	// registration
	template<typename T>
	void RegisterEntity(const Entity::Key& key);
	void UnregisterEntity(const Entity::Key& key);
	void UnregisterAll();
	bool HasRegisteredKey(const Entity::Key& key) const;

	// deserialization & generation
	void LoadEntity(ISerializableEntity& entity);
	std::unique_ptr<ISerializableEntity> GenerateEntity(const Entity::Key& key) const;

private:
	EntityAggregationDeserializer();
	EntityAggregationDeserializer(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer& operator=(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer(EntityAggregationDeserializer&&) = delete;
	EntityAggregationDeserializer& operator=(EntityAggregationDeserializer&&) = delete;

	void LoadWithParentKey(ISerializableEntity& entity, const Entity::Key& parentKey = "");

	static EntityAggregationDeserializer* instance_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;
	mutable std::map<Entity::Key, std::function<std::unique_ptr<ISerializableEntity>(void)>> keyToEntityMap_;
};

#include "EntityAggregationDeserializer.hpp"

} // end namespace entity
#endif // entity_entityaggregationdeserializer_h

