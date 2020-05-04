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

#include "EntityRegistry.h"
#include "ISerializableEntity.h"

namespace entity {

class ENTITY_DLL_EXPORT EntityAggregationDeserializer
{
public:
	virtual ~EntityAggregationDeserializer();

	static EntityAggregationDeserializer* GetInstance();
	static void ResetInstance();

	//registration
	EntityRegistry<ISerializableEntity>& GetRegistry();

	// structure
	void LoadSerializationStructure(const std::string& pathToJSON);
	bool HasSerializationStructure() const;
	boost::property_tree::ptree GetSerializationStructure() const;

	// deserialization 
	void LoadEntity(ISerializableEntity& entity);

private:
	EntityAggregationDeserializer();
	EntityAggregationDeserializer(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer& operator=(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer(EntityAggregationDeserializer&&) = delete;
	EntityAggregationDeserializer& operator=(EntityAggregationDeserializer&&) = delete;

	void LoadWithParentKey(ISerializableEntity& entity, const Entity::Key& parentKey = "");

	static EntityAggregationDeserializer* instance_;

	EntityRegistry<ISerializableEntity> registry_;
	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;
	mutable std::map<Entity::Key, std::function<std::unique_ptr<ISerializableEntity>(void)>> keyToEntityMap_;
};

} // end namespace entity
#endif // entity_entityaggregationdeserializer_h

