#ifndef entity_h
#define entity_h

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include "config.h"

namespace entity {

class EntityAggregationDeserializer;
class EntityAggregationSerializer;

class ENTITY_DLL_EXPORT Entity
{
public:
	using MemberKeys = std::vector<std::string>;
	using Members = std::map<std::string, std::shared_ptr<Entity>>;

	friend class EntityAggregationSerializer;
	friend class EntityAggregationDeserializer;

public:
	Entity();
	Entity(const Entity& other);
	Entity& operator=(const Entity& other);
	Entity(Entity&& other);
	Entity& operator=(Entity&& other);
	virtual ~Entity();

	std::string GetKey() const;
	void SetKey(const std::string& key);

	virtual void Save(boost::property_tree::ptree& tree, const std::string& path) const;
	virtual void Load(boost::property_tree::ptree& tree, const std::string& path);

protected:
	const Members& GetAggregatedMembers() const;
	std::shared_ptr<Entity> GetAggregatedMember(const std::string& key);
	MemberKeys GetAggregatedMemberKeys() const;

	virtual void AggregateMember(const std::string& key, std::shared_ptr<Entity> copied);
	virtual void AggregateMember(std::shared_ptr<Entity> copied);

private:
	std::string key_;
	Members members_;
};

} // end namespace entity

#endif // entity_h

