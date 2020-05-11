#include "ISerializableEntity.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "Entities/Entity.h"
#include "EntityDeserializer.h"

using entity::Entity;
using filesystem_adapters::EntityDeserializer;
using filesystem_adapters::ISerializableEntity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

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

	EntityDeserializer* deserializer = EntityDeserializer::GetInstance();
	if (deserializer->GetRegistry().HasRegisteredKey(key))
	{
		std::unique_ptr<ISerializableEntity> entity = deserializer->GetRegistry().GenerateEntity(key);

		deserializer->LoadEntity(*entity);
		members[key] = std::move(entity);
	}
	return members[key];
}
