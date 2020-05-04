#ifndef filesystem_adapters_entityaggregationserializer_h
#define filesystem_adapters_entityaggregationserializer_h

#include <filesystem>
#include <string>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "ISerializableEntity.h"

namespace filesystem_adapters {

class FILESYSTEM_ADAPTERS_DLL_EXPORT EntityAggregationSerializer
{
public:
	virtual ~EntityAggregationSerializer();

	static EntityAggregationSerializer* GetInstance();
	static void ResetInstance();

	void Serialize(const ISerializableEntity& entity);

	// structure
	entity::EntityHierarchy& GetHierarchy();

private:
	EntityAggregationSerializer();
	EntityAggregationSerializer(const EntityAggregationSerializer&) = delete;
	EntityAggregationSerializer& operator=(const EntityAggregationSerializer&) = delete;
	EntityAggregationSerializer(EntityAggregationSerializer&&) = delete;
	EntityAggregationSerializer& operator=(EntityAggregationSerializer&&) = delete;

	void SerializeWithParentKey(const ISerializableEntity& entity, const entity::Entity::Key& parentKey = "");

	static EntityAggregationSerializer* instance_;

	entity::EntityHierarchy hierarchy_;
};

} // end namespace filesystem_adapters
#endif // filesystem_adapters_entityaggregationserializer_h

