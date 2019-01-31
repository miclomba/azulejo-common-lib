#ifndef global_entity
#define global_entity

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include "config.h"

namespace global {

class EntityAggregationDeserializer;
class EntityAggregationSerializer;

class X_DLL_EXPORT Entity
{
public:
	using MemberKeys = std::vector<std::string>;
	using Members = std::map<std::string, std::shared_ptr<Entity>>;

	friend class EntityAggregationSerializer;
	friend class EntityAggregationDeserializer;

	const std::string GetKey() const;
	void SetKey(const std::string& key);

	virtual void Save(boost::property_tree::ptree& tree, const std::string& path) const;
	virtual void Load(boost::property_tree::ptree& tree, const std::string& path);

public:
	Entity();
	Entity(const Entity& other);
	Entity& operator=(const Entity& other);
	Entity(Entity&& other);
	Entity& operator=(Entity&& other);
	virtual ~Entity();

protected:
	const Members& GetAggregatedMembers() const;
	Entity& GetAggregatedMember(const std::string& key);
	virtual void AggregateMember(std::shared_ptr<Entity> copied);
	const MemberKeys GetAggregatedMemberKeys() const;

private:
	std::string key_;
	Members members_;
};

} // namespace global

#endif // global_entity

