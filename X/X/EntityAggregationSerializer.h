#ifndef global_entity_aggregation_serializer_h
#define global_entity_aggregation_serializer_h

#include <filesystem>
#include <memory>
#include <string>

#include <boost/property_tree/ptree.hpp>

#include <memory>
#include <string>

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT EntityAggregationSerializer
{
public:
	virtual ~EntityAggregationSerializer();

	static EntityAggregationSerializer* GetInstance();
	static void ResetInstance();

	void LoadSerializationStructure(const std::string& pathToJSON);

	void Serialize(const Entity& entity);
	std::shared_ptr<Entity> Deserialize(const std::string& key);

	bool HasSerializationStructure() const;

private:
	static EntityAggregationSerializer* instance_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;

	EntityAggregationSerializer();
	EntityAggregationSerializer(const EntityAggregationSerializer&) = delete;
	EntityAggregationSerializer& operator=(const EntityAggregationSerializer&) = delete;
	EntityAggregationSerializer(EntityAggregationSerializer&&) = delete;
	EntityAggregationSerializer& operator=(EntityAggregationSerializer&&) = delete;
};

} // end namespace global
#endif // global_entity_aggregation_serializer_h

