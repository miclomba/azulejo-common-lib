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

#include "EntityHierarchy.h"
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
	EntityHierarchy& GetHierarchy();

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
	EntityHierarchy hierarchy_;
};

} // end namespace entity
#endif // entity_entityaggregationdeserializer_h

