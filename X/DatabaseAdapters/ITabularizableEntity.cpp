#include "ITabularizableEntity.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "Entities/Entity.h"
#include "EntityAggregationDetabularizer.h"

using database_adapters::EntityAggregationDetabularizer;
using database_adapters::ITabularizableEntity;
using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

ITabularizableEntity::ITabularizableEntity() = default;
ITabularizableEntity::~ITabularizableEntity() = default;

ITabularizableEntity::ITabularizableEntity(const ITabularizableEntity&) = default;
ITabularizableEntity& ITabularizableEntity::operator=(const ITabularizableEntity&) = default;
ITabularizableEntity::ITabularizableEntity(ITabularizableEntity&&) = default;
ITabularizableEntity& ITabularizableEntity::operator=(ITabularizableEntity&&) = default;

SharedEntity& ITabularizableEntity::GetAggregatedMember(const Key& key) const
{
	MemberMap& members = Entity::GetAggregatedMembers();

	auto found = members.find(key);
	if (found == members.cend())
		throw std::runtime_error("Could not find entity using key=" + key + " because this key is not in use.");
	
	if (members[key])
		return members[key];

	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();
	if (detabularizer->GetRegistry().HasRegisteredKey(key))
	{
		std::unique_ptr<ITabularizableEntity> entity = detabularizer->GetRegistry().GenerateEntity(key);

		detabularizer->LoadEntity(*entity);
		members[key] = std::move(entity);
	}
	return members[key];
}
