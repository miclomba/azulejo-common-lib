#include "DatabaseAdapters/IPersistableEntity.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "Entities/Entity.h"
#include "DatabaseAdapters/EntityLoader.h"

using database_adapters::EntityLoader;
using database_adapters::IPersistableEntity;
using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

IPersistableEntity::IPersistableEntity() = default;
IPersistableEntity::~IPersistableEntity() = default;

IPersistableEntity::IPersistableEntity(const IPersistableEntity &) = default;
IPersistableEntity &IPersistableEntity::operator=(const IPersistableEntity &) = default;
IPersistableEntity::IPersistableEntity(IPersistableEntity &&) = default;
IPersistableEntity &IPersistableEntity::operator=(IPersistableEntity &&) = default;

SharedEntity &IPersistableEntity::GetAggregatedMember(const Key &key) const
{
	MemberMap &members = Entity::GetAggregatedMembers();

	if (auto found = members.find(key); found == members.cend())
		throw std::runtime_error("Could not find entity using key=" + key + " because this key is not in use.");

	if (members[key])
		return members[key];

	if (auto loader = EntityLoader::GetInstance(); loader->GetRegistry().HasRegisteredKey(key))
	{
		std::unique_ptr<IPersistableEntity> entity = loader->GetRegistry().GenerateEntity(key);

		loader->LoadEntity(*entity);
		members[key] = std::move(entity);
	}
	return members[key];
}
