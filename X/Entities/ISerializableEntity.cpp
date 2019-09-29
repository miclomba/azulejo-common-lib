#include "ISerializableEntity.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "EntityAggregationDeserializer.h"

using entity::Entity;
using entity::ISerializableEntity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;
using SerializableMemberMap = ISerializableEntity::SerializableMemberMap;

namespace entity {

ISerializableEntity::ISerializableEntity() = default;
ISerializableEntity::~ISerializableEntity() = default;

ISerializableEntity::ISerializableEntity(const ISerializableEntity&) = default;
ISerializableEntity& ISerializableEntity::operator=(const ISerializableEntity&) = default;
ISerializableEntity::ISerializableEntity(ISerializableEntity&&) = default;
ISerializableEntity& ISerializableEntity::operator=(ISerializableEntity&&) = default;

SharedEntity& ISerializableEntity::GetAggregatedMember(const Key& key) const
{
	MemberMap& members = Entity::GetAggregatedMembers();

	auto found = members.find(key);
	if (found == members.cend())
		throw std::runtime_error("Could not find entity using key=" + key + " because this key is not in use.");
	
	if (members[key])
		return members[key];

	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();
	if (deserializer->HasSerializationKey(key))
	{
		std::unique_ptr<ISerializableEntity> entity = deserializer->GenerateEntity(key);

		deserializer->Deserialize(*entity);
		members[key] = std::move(entity);
	}
	return members[key];
}

SerializableMemberMap ISerializableEntity::GetAggregatedMembers() const
{
	SerializableMemberMap members;

	const MemberMap& allMembers = Entity::GetAggregatedMembers();
	for (const std::pair<Key, SharedEntity>& member : allMembers)
	{
		if (member.second && std::dynamic_pointer_cast<ISerializableEntity>(member.second))
			members.insert(std::make_pair(member.first, std::static_pointer_cast<ISerializableEntity>(member.second)));
	}
	return members;
}

} // end namespace entity
