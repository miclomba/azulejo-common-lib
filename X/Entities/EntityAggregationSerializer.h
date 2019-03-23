#ifndef entity_entityaggregationserializer_h
#define entity_entityaggregationserializer_h

#include <filesystem>
#include <string>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "ISerializableEntity.h"

namespace entity {

class ENTITY_DLL_EXPORT EntityAggregationSerializer
{
public:
	virtual ~EntityAggregationSerializer();

	static EntityAggregationSerializer* GetInstance();
	static void ResetInstance();

	void Serialize(const ISerializableEntity& entity);

	void SetSerializationPath(const std::string& pathToJSON);
	std::string GetSerializationPath() const;

private:
	EntityAggregationSerializer();
	EntityAggregationSerializer(const EntityAggregationSerializer&) = delete;
	EntityAggregationSerializer& operator=(const EntityAggregationSerializer&) = delete;
	EntityAggregationSerializer(EntityAggregationSerializer&&) = delete;
	EntityAggregationSerializer& operator=(EntityAggregationSerializer&&) = delete;

	void SerializeWithParentKey(const ISerializableEntity& entity, const std::string& parentKey = "");

	static EntityAggregationSerializer* instance_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;
};

} // end namespace entity
#endif // entity_entityaggregationserializer_h

