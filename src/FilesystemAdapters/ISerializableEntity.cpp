#include "FilesystemAdapters/ISerializableEntity.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "Entities/Entity.h"
#include "FilesystemAdapters/EntityDeserializer.h"

using entity::Entity;
using filesystem_adapters::EntityDeserializer;
using filesystem_adapters::ISerializableEntity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

ISerializableEntity::ISerializableEntity() = default;
ISerializableEntity::~ISerializableEntity() noexcept = default;

ISerializableEntity::ISerializableEntity(const ISerializableEntity &) = default;
ISerializableEntity &ISerializableEntity::operator=(const ISerializableEntity &) = default;
ISerializableEntity::ISerializableEntity(ISerializableEntity &&) noexcept = default;
ISerializableEntity &ISerializableEntity::operator=(ISerializableEntity &&) noexcept = default;

SharedEntity &ISerializableEntity::GetAggregatedMember(const Key &key) const
{
	MemberMap &members = Entity::GetAggregatedMembers();

	if (auto found = members.find(key); found == members.cend())
		throw std::runtime_error("Could not find entity using key=" + key + " because this key is not in use.");

	if (members[key])
		return members[key];

	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();
	if (deserializer->GetRegistry().HasRegisteredKey(key))
	{
		std::unique_ptr<ISerializableEntity> entity = deserializer->GetRegistry().GenerateEntity(key);

		deserializer->LoadEntity(*entity);
		members[key] = std::move(entity);
	}
	return members[key];
}
