#ifndef global_entity_aggregation_deserializer_h
#define global_entity_aggregation_deserializer_h

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include <boost/property_tree/ptree.hpp>

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT EntityAggregationDeserializer
{
public:
	virtual ~EntityAggregationDeserializer();

	static EntityAggregationDeserializer* GetInstance();
	static void ResetInstance();

	void LoadSerializationStructure(const std::string& pathToJSON);
	bool HasSerializationStructure() const;

	void Deserialize(Entity& entity);

	template<typename T>
	void RegisterEntity(const std::string& key);
	void UnregisterEntity(const std::string& key);

	std::unique_ptr<Entity> GenerateEntity(const std::string& key);

private:
	EntityAggregationDeserializer();
	EntityAggregationDeserializer(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer& operator=(const EntityAggregationDeserializer&) = delete;
	EntityAggregationDeserializer(EntityAggregationDeserializer&&) = delete;
	EntityAggregationDeserializer& operator=(EntityAggregationDeserializer&&) = delete;

	std::string GetKeyPath(const std::string& key, const boost::property_tree::ptree& tree) const;
	void DeserializeWithParentKey(Entity& entity, const std::string& parentKey = "");

	static EntityAggregationDeserializer* instance_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;
	std::map<std::string, std::function<std::unique_ptr<Entity>(void)>> keyToEntityMap_;
};

#include "EntityAggregationDeserializer.hpp"

} // end namespace global
#endif // global_entity_aggregation_deserializer_h

