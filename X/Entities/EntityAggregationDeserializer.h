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
	bool HasSerializationKey(const Entity::Key& key) const;

	void Deserialize(ISerializableEntity& entity);

	template<typename T>
	void RegisterEntity(const Entity::Key& key);
	void UnregisterEntity(const Entity::Key& key);

	std::unique_ptr<ISerializableEntity> GenerateEntity(const Entity::Key& key) const;

private:
	EntityAggregationDeserializer();
	EntityAggregationDeserializer(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer& operator=(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer(EntityAggregationDeserializer&&) = delete;
	EntityAggregationDeserializer& operator=(EntityAggregationDeserializer&&) = delete;

	void DeserializeWithParentKey(ISerializableEntity& entity, const Entity::Key& parentKey = "");

	static EntityAggregationDeserializer* instance_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;
	mutable std::map<Entity::Key, std::function<std::unique_ptr<ISerializableEntity>(void)>> keyToEntityMap_;
};

#include "EntityAggregationDeserializer.hpp"

} // end namespace entity
#endif // entity_entityaggregationdeserializer_h

