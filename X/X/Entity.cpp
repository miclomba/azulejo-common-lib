#include "Entity.h"
#include "EntityAggregationDeserializer.h"

#include <iostream>
#include <queue>

#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

namespace entity {

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

std::string Entity::GetKey() const
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

MemberKeys Entity::GetAggregatedMemberKeys() const
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

std::shared_ptr<Entity> Entity::GetAggregatedMember(const std::string& key)
{
	auto found = members_.find(key);
	if (found != members_.cend())
	{
		if (!members_[key])
		{
			auto deserializer = EntityAggregationDeserializer::GetInstance();
			if (deserializer->HasSerializationKey(key))
			{
				std::unique_ptr<Entity> entity = deserializer->GenerateEntity(key);
				deserializer->Deserialize(*entity);
				members_[key] = std::move(entity);
			}
		}
		return members_[key];
	}

	throw std::runtime_error("Could not find entity using key=" + key + " because this key is not in use.");
}

void Entity::AggregateMember(const std::string& key, std::shared_ptr<Entity> entity)
{
	auto found = members_.find(key);
	if (found != members_.cend())
		throw std::runtime_error("Cannot aggregate entity using key=" + key + " because this key is already in use.");

	if (entity)
		entity->SetKey(key);

	members_[key] = std::move(entity);
}

void Entity::AggregateMember(std::shared_ptr<Entity> entity)
{
	if (!entity)
		throw std::runtime_error("Entity is nullptr when aggregating entity");

	std::string key = entity->GetKey();
	AggregateMember(key, std::move(entity));
}

} // end namespace entity