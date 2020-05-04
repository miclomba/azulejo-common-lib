#ifndef filesystem_adapters_entityaggregationdeserializer_h
#define filesystem_adapters_entityaggregationdeserializer_h

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/Entity.h"
#include "Entities/EntityHierarchy.h"
#include "Entities/EntityRegistry.h"
#include "ISerializableEntity.h"

namespace filesystem_adapters {

class FILESYSTEM_ADAPTERS_DLL_EXPORT EntityAggregationDeserializer
{
public:
	virtual ~EntityAggregationDeserializer();

	static EntityAggregationDeserializer* GetInstance();
	static void ResetInstance();

	//registration
	entity::EntityRegistry<ISerializableEntity>& GetRegistry();

	// structure
	entity::EntityHierarchy& GetHierarchy();

	// deserialization 
	void LoadEntity(ISerializableEntity& entity);

private:
	EntityAggregationDeserializer();
	EntityAggregationDeserializer(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer& operator=(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer(EntityAggregationDeserializer&&) = delete;
	EntityAggregationDeserializer& operator=(EntityAggregationDeserializer&&) = delete;

	void LoadWithParentKey(ISerializableEntity& entity, const entity::Entity::Key& parentKey = "");

	static EntityAggregationDeserializer* instance_;

	entity::EntityRegistry<ISerializableEntity> registry_;
	entity::EntityHierarchy hierarchy_;
};

} // end namespace filesystem_adapters
#endif // filesystem_adapters_entityaggregationdeserializer_h

