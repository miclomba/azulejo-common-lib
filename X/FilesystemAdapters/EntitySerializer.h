#ifndef filesystem_adapters_entityserializer_h
#define filesystem_adapters_entityserializer_h

#include <filesystem>
#include <string>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "ISerializableEntity.h"

namespace filesystem_adapters {

class FILESYSTEM_ADAPTERS_DLL_EXPORT EntitySerializer
{
public:
	virtual ~EntitySerializer();

	static EntitySerializer* GetInstance();
	static void ResetInstance();

	void Serialize(const ISerializableEntity& entity);

	// structure
	entity::EntityHierarchy& GetHierarchy();

private:
	EntitySerializer();
	EntitySerializer(const EntitySerializer&) = delete;
	EntitySerializer& operator=(const EntitySerializer&) = delete;
	EntitySerializer(EntitySerializer&&) = delete;
	EntitySerializer& operator=(EntitySerializer&&) = delete;

	void SerializeWithParentKey(const ISerializableEntity& entity, const entity::Entity::Key& parentKey = "");

	static EntitySerializer* instance_;

	entity::EntityHierarchy hierarchy_;
};

} // end namespace filesystem_adapters
#endif // filesystem_adapters_entityserializer_h

