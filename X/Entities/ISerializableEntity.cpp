#include "ISerializableEntity.h"

#include <utility>

#include "EntityAggregationDeserializer.h"

namespace
{
entity::ISerializableEntity::Members GetAggregatedSerializableMembers(const entity::Entity::MemberMap& allMembers)
{
	entity::ISerializableEntity::Members members;

	for (auto& member : allMembers)
	{
		if (member.second && dynamic_cast<entity::ISerializableEntity*>(member.second.get()))
			members.insert(std::make_pair(member.first, std::static_pointer_cast<entity::ISerializableEntity>(member.second)));
	}
	return members;
}
}

namespace entity {

ISerializableEntity::ISerializableEntity() = default;
ISerializableEntity::~ISerializableEntity() = default;

ISerializableEntity::ISerializableEntity(const ISerializableEntity&) = default;
ISerializableEntity& ISerializableEntity::operator=(const ISerializableEntity&) = default;
ISerializableEntity::ISerializableEntity(ISerializableEntity&&) = default;
ISerializableEntity& ISerializableEntity::operator=(ISerializableEntity&&) = default;

std::shared_ptr<Entity> ISerializableEntity::GetAggregatedMember(const std::string& key)
{
	Entity::MemberMap& members = Entity::GetAggregatedMembers();
	auto found = members.find(key);
	if (found != members.cend())
	{
		if (!members[key])
		{
			auto deserializer = EntityAggregationDeserializer::GetInstance();
			if (deserializer->HasSerializationKey(key))
			{
				std::unique_ptr<ISerializableEntity> entity = deserializer->GenerateEntity(key);

				deserializer->Deserialize(*entity);
				members[key] = std::move(entity);
			}
		}
		return members[key];
	}

	throw std::runtime_error("Could not find entity using key=" + key + " because this key is not in use.");
}

ISerializableEntity::Members ISerializableEntity::GetAggregatedMembers()
{
	return GetAggregatedSerializableMembers(Entity::GetAggregatedMembers());
}

const ISerializableEntity::Members ISerializableEntity::GetAggregatedMembers() const
{
	return GetAggregatedSerializableMembers(Entity::GetAggregatedMembers());
}

} // end namespace entity
