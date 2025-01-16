#ifndef filesystem_adapters_entitydeserializer_h
#define filesystem_adapters_entitydeserializer_h

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "Config/filesystem.h"

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/Entity.h"
#include "Entities/EntityHierarchy.h"
#include "Entities/EntityRegistry.h"
#include "ISerializableEntity.h"

namespace filesystem_adapters {

class FILESYSTEM_ADAPTERS_DLL_EXPORT EntityDeserializer
{
public:
	virtual ~EntityDeserializer();

	static EntityDeserializer* GetInstance();
	static void ResetInstance();

	//registration
	entity::EntityRegistry<ISerializableEntity>& GetRegistry();

	// structure
	entity::EntityHierarchy& GetHierarchy();

	// deserialization 
	void LoadEntity(ISerializableEntity& entity);

private:
	EntityDeserializer();
	EntityDeserializer(const EntityDeserializer&) = delete;
	EntityDeserializer& operator=(const EntityDeserializer&) = delete;
	EntityDeserializer(EntityDeserializer&&) = delete;
	EntityDeserializer& operator=(EntityDeserializer&&) = delete;

	void LoadWithParentKey(ISerializableEntity& entity, const entity::Entity::Key& parentKey = "");

	static EntityDeserializer* instance_;

	entity::EntityRegistry<ISerializableEntity> registry_;
	entity::EntityHierarchy hierarchy_;
};

} // end namespace filesystem_adapters
#endif // filesystem_adapters_entitydeserializer_h

