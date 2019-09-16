#include "Entity.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "EntityAggregationDeserializer.h"

using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace entity {

Entity::Entity() = default;

Entity::Entity(Entity&&) = default;
Entity::Entity(const Entity& other) :
	key_(other.key_)
{
	for (const std::pair<Key,SharedEntity>& e : other.membersMap_)
		membersMap_[e.first] = std::make_shared<Entity>(*e.second);
}

Entity& Entity::operator=(Entity&&) = default;
Entity& Entity::operator=(const Entity& other)
{
	if (this == &other)
		return *this;

	key_ = other.key_;

	membersMap_.clear();
	for (const std::pair<Key,SharedEntity>& e : other.membersMap_)
		membersMap_[e.first] = std::make_shared<Entity>(*e.second);
}

Entity::~Entity() = default;

Key Entity::GetKey() const
{
	if (key_.empty())
		throw std::runtime_error("Entity key has not been set when getting key");
	return key_;
}

void Entity::SetKey(const Key& key)
{
	key_ = key;
}

std::vector<Key> Entity::GetAggregatedMemberKeys() const
{
	std::vector<Key> keys;
	for (const std::pair<Key, SharedEntity>& e : membersMap_)
		keys.push_back(e.first);
	return keys;
}

std::map<Key,SharedEntity>& Entity::GetAggregatedMembers() const
{
	return membersMap_;
}

SharedEntity Entity::GetAggregatedMember(const Key& key) const
{
	auto found = membersMap_.find(key);
	if (found != membersMap_.cend())
		return membersMap_[key];

	throw std::runtime_error("Could not find entity using key=" + key + " because this key is not in use.");
}

void Entity::AggregateMember(const Key& key)
{
	auto found = membersMap_.find(key);
	if (found != membersMap_.cend())
		throw std::runtime_error("Cannot aggregate entity using key=" + key + " because this key is already in use.");

	membersMap_[key] = nullptr;
}

void Entity::AggregateMember(SharedEntity sharedObj)
{
	if (!sharedObj)
		throw std::runtime_error("Entity is nullptr when aggregating entity");

	Key key = sharedObj->GetKey();

	auto found = membersMap_.find(key);
	if (found != membersMap_.cend())
		throw std::runtime_error("Cannot aggregate entity using key=" + key + " because this key is already in use.");

	membersMap_[key] = std::move(sharedObj);
}

} // end namespace entity
