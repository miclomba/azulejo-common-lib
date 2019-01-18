#include "Entity.h"
#include "EntityAggregationSerializer.h"

#include <iostream>
#include <queue>

#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

namespace global {

using MemberKeys = Entity::MemberKeys;
using Members = Entity::Members;

Entity::Entity() = default;

Entity::Entity(Entity&&) = default;
Entity::Entity(const Entity& other)
{
	for (auto& e : other.members_)
		members_[e.first] = std::make_shared<Entity>(*e.second);
}

Entity& Entity::operator=(Entity&&) = default;
Entity& Entity::operator=(const Entity& other)
{
	if (this == &other)
		return *this;

	members_.clear();
	for (auto& e : other.members_)
		members_[e.first] = std::make_shared<Entity>(*e.second);
}

Entity::~Entity() = default;

const std::string Entity::GetKey() const
{
	if (key_.empty())
		throw std::runtime_error("Entity key has not been set when getting key");
	return key_;
}

void Entity::SetKey(const std::string& key)
{
	key_ = key;
}

void Entity::Save(ptree& tree, const std::string& path) const
{
}

void Entity::Load(ptree& tree, const std::string& path)
{
}

const MemberKeys Entity::GetAggregatedMemberKeys() const
{
	MemberKeys keys;
	for (auto& e : members_)
		keys.push_back(e.first);
	return keys;
}

const Members& Entity::GetAggregatedMembers() const
{
	return members_;
}

Entity& Entity::GetAggregatedMember(const std::string& key)
{
	auto found = members_.find(key);
	if (found != members_.cend())
	{
		if (!members_[key])
			members_[key] = EntityAggregationSerializer::GetInstance()->Deserialize(key);
		return *members_[key];
	}

	throw std::runtime_error("Could not find entity using key=" + key + " because this key is not in use.");
}

void Entity::AggregateMember(std::shared_ptr<Entity> entity)
{
	std::string key = entity->GetKey();
	auto found = members_.find(key);
	if (found != members_.cend())
		throw std::runtime_error("Cannot aggregate entity using key=" + key + " because this key is already in use.");

	members_[key] = std::move(entity);
}

}