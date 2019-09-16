#ifndef entity_iserializableentity_h
#define entity_iserializableentity_h

#include <map>
#include <memory>
#include <string>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entity.h"

namespace entity {

class EntityAggregationDeserializer;
class EntityAggregationSerializer;

class ENTITY_DLL_EXPORT ISerializableEntity : public Entity
{
public:
	using SharedSerializable = std::shared_ptr<ISerializableEntity>;
	using SerializableMemberMap = std::map<Key, SharedSerializable>;

	friend class EntityAggregationSerializer;
	friend class EntityAggregationDeserializer;

public:
	ISerializableEntity();
	virtual ~ISerializableEntity();

	ISerializableEntity(const ISerializableEntity&);
	ISerializableEntity& operator=(const ISerializableEntity&);
	ISerializableEntity(ISerializableEntity&&);
	ISerializableEntity& operator=(ISerializableEntity&&);

	virtual void Save(boost::property_tree::ptree& tree, const std::string& path) const = 0;
	virtual void Load(boost::property_tree::ptree& tree, const std::string& path) = 0;

protected:
	SharedEntity GetAggregatedMember(const Key& key) const override;

	SerializableMemberMap GetAggregatedMembers() const;
};

} // end namespace entity

#endif // entity_iserializableentity_h

