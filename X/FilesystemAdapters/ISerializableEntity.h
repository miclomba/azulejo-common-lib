#ifndef filesystem_adapters_iserializableentity_h
#define filesystem_adapters_iserializableentity_h

#include <map>
#include <memory>
#include <string>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/Entity.h"

namespace filesystem_adapters {

class EntityAggregationDeserializer;
class EntityAggregationSerializer;

class FILESYSTEM_ADAPTERS_DLL_EXPORT ISerializableEntity : public entity::Entity
{
public:
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
	SharedEntity& GetAggregatedMember(const Key& key) const override;
};

} // end namespace filesystem_adapters

#endif // filesystem_adapters_iserializableentity_h

