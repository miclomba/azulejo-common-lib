#include "Entity.h"

#include <iostream>
#include <queue>

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

const MemberKeys Entity::GetMemberKeys() const
{
	MemberKeys keys;
	for (auto& e : members_)
		keys.push_back(e.first);
	return keys;
}

const Members& Entity::GetMembers() const
{
	return members_;
}

Entity& Entity::GetMember(const std::string& key)
{
	auto found = members_.find(key);
	if (found != members_.cend())
		return *members_[key];

	throw std::runtime_error("Could not find entity using key=" + key + " because this key is not in use.");
}

void Entity::AggregateMember(const std::string& key, std::shared_ptr<Entity> entity)
{
	auto found = members_.find(key);
	if (found != members_.cend())
		throw std::runtime_error("Cannot aggregate entity using key=" + key + " because this key is already in use.");

	members_[key] = std::move(entity);
}

}