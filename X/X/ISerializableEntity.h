#ifndef entity_iserializableentity_h
#define entity_iserializableentity_h

#include <string>

#include <boost/property_tree/ptree.hpp>

#include "config.h"
#include "Entity.h"

namespace entity {

class EntityAggregationDeserializer;
class EntityAggregationSerializer;

class ENTITY_DLL_EXPORT ISerializableEntity : public Entity
{
public:
	using Members = std::map<std::string, std::shared_ptr<ISerializableEntity>>;

	friend class EntityAggregationSerializer;
	friend class EntityAggregationDeserializer;

public:
	virtual ~ISerializableEntity();

	virtual void Save(boost::property_tree::ptree& tree, const std::string& path) const = 0;
	virtual void Load(boost::property_tree::ptree& tree, const std::string& path) = 0;

protected:
	std::shared_ptr<Entity> GetAggregatedMember(const std::string& key) override;

	Members GetAggregatedMembers();
	const Members GetAggregatedMembers() const;
};

} // end namespace entity

#endif // entity_iserializableentity_h

