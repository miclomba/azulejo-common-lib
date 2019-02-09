#ifndef entity_h
#define entity_h

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "config.h"

namespace entity {

class ENTITY_DLL_EXPORT Entity
{
public:
	using MemberKeys = std::vector<std::string>;
	using Members = std::map<std::string, std::shared_ptr<Entity>>;

public:
	virtual ~Entity();

	Entity(const Entity& other);
	Entity& operator=(const Entity& other);
	Entity(Entity&& other);
	Entity& operator=(Entity&& other);

	std::string GetKey() const;
	void SetKey(const std::string& key);

protected:
	Entity();

	// make virtual so that classes such as ISerializableEntity can override for lazy loading
	virtual std::shared_ptr<Entity> GetAggregatedMember(const std::string& key);

	Members& GetAggregatedMembers();
	const Members& GetAggregatedMembers() const;

	MemberKeys GetAggregatedMemberKeys() const;

	void AggregateMember(const std::string& key, std::shared_ptr<Entity> copied);
	void AggregateMember(std::shared_ptr<Entity> copied);

private:
	std::string key_;
	Members members_;
};

} // end namespace entity

#endif // entity_h

