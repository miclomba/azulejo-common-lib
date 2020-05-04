#include "config.h"

#include <memory>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

#include "Entities/ISerializableEntity.h"
#include "Entities/EntityRegistry.h"

namespace pt = boost::property_tree;

using entity::Entity;
using entity::EntityRegistry;
using entity::ISerializableEntity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
const std::string ENTITY_1A = "entity_1a";

struct TypeA : public ISerializableEntity 
{
	void Save(pt::ptree& tree, const std::string& path) const override {};
	void Load(pt::ptree& tree, const std::string& path) override {};
};
} // end namespace anonymous

TEST(EntityRegistry, HasRegisteredKeyFalse)
{
	EntityRegistry<TypeA> registry;
	EXPECT_FALSE(registry.HasRegisteredKey(ENTITY_1A));
}

TEST(EntityRegistry, HasRegisteredKeyTrue)
{
	EntityRegistry<TypeA> registry;
	EXPECT_NO_THROW(registry.RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(registry.HasRegisteredKey(ENTITY_1A));
}

TEST(EntityRegistry, RegisterEntity)
{
	EntityRegistry<TypeA> registry;
	EXPECT_FALSE(registry.HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(registry.RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(registry.HasRegisteredKey(ENTITY_1A));
}

TEST(EntityRegistry, RegisterEntityThrowsUsingEmptyKey)
{
	EntityRegistry<TypeA> registry;
	EXPECT_THROW(registry.RegisterEntity<TypeA>(""), std::runtime_error);
}

TEST(EntityRegistry, RegisterEntityThrowsUsingDuplicateKey)
{
	EntityRegistry<TypeA> registry;
	EXPECT_FALSE(registry.HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(registry.RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(registry.HasRegisteredKey(ENTITY_1A));
	EXPECT_THROW(registry.RegisterEntity<TypeA>(ENTITY_1A), std::runtime_error);
}

TEST(EntityRegistry, UnregisterEntity)
{
	EntityRegistry<TypeA> registry;
	EXPECT_FALSE(registry.HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(registry.RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(registry.HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(registry.UnregisterEntity(ENTITY_1A));
	EXPECT_FALSE(registry.HasRegisteredKey(ENTITY_1A));
}

TEST(EntityRegistry, UnregisterEntityThrows)
{
	EntityRegistry<TypeA> registry;
	EXPECT_FALSE(registry.HasRegisteredKey(ENTITY_1A));
	EXPECT_THROW(registry.UnregisterEntity(ENTITY_1A), std::runtime_error);
}

TEST(EntityRegistry, UnregisterAll)
{
	EntityRegistry<TypeA> registry;
	EXPECT_FALSE(registry.HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(registry.RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(registry.HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(registry.UnregisterAll());
	EXPECT_FALSE(registry.HasRegisteredKey(ENTITY_1A));
}

TEST(EntityRegistry, GenerateEntity)
{
	EntityRegistry<TypeA> registry;
	registry.RegisterEntity<TypeA>(ENTITY_1A);
	std::unique_ptr<ISerializableEntity> entity = registry.GenerateEntity(ENTITY_1A);
	EXPECT_TRUE(entity);
	EXPECT_TRUE(entity->GetKey() == ENTITY_1A);
}

TEST(EntityRegistry, GenerateEntityThrows)
{
	EntityRegistry<TypeA> registry;
	EXPECT_FALSE(registry.HasRegisteredKey(ENTITY_1A));
	EXPECT_THROW(registry.GenerateEntity(ENTITY_1A), std::runtime_error);
}

