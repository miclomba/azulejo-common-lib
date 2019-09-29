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
	using Key = std::string;
	using SharedEntity = std::shared_ptr<Entity>;
	using MemberMap = std::map<Key, SharedEntity>;

public:
	virtual ~Entity();

	Entity(const Entity& other);
	Entity& operator=(const Entity& other);
	Entity(Entity&& other);
	Entity& operator=(Entity&& other);

	Key GetKey() const;
	void SetKey(const Key& key);

protected:
	Entity();

	template<class T>
	SharedEntity& GetAggregatedMember(const Key& key) const;
	// make virtual so that classes such as ISerializableEntity can override for lazy loading
	virtual SharedEntity& GetAggregatedMember(const Key& key) const;

	std::map<Key, SharedEntity>& GetAggregatedMembers() const;

	template<class T = Entity>
	std::vector<Key> GetAggregatedMemberKeys() const;

	void AggregateMember(const Key& key);
	void AggregateMember(SharedEntity sharedObj);

	void RemoveMember(const Key& key);
	void RemoveMember(SharedEntity sharedObj);

private:
	Key key_;
	mutable std::map<Key, SharedEntity> membersMap_;
};

#include "Entity.hpp"

} // end namespace entity

#endif // entity_h

