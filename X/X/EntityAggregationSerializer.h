#ifndef global_entity_aggregation_serializer_h
#define global_entity_aggregation_serializer_h

#include <filesystem>
#include <string>

#include <boost/property_tree/ptree.hpp>

#include "config.h"
#include "Entity.h"

namespace global {

class X_DLL_EXPORT EntityAggregationSerializer
{
public:
	virtual ~EntityAggregationSerializer();

	static EntityAggregationSerializer* GetInstance();
	static void ResetInstance();

	void Serialize(const Entity& entity, const std::string& parentKey = "");

	void SetSerializationPath(const std::string& pathToJSON);
	std::string GetSerializationPath() const;

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

