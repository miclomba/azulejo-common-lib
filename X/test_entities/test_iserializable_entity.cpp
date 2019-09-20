#include "config.h"

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

#include "Entities/ISerializableEntity.h"

namespace pt = boost::property_tree;

using entity::Entity;
using entity::ISerializableEntity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;
using SerializationMembersMap = ISerializableEntity::SerializableMemberMap;

namespace
{
const std::string ROOT_KEY = "root";
const std::string SERIALIZABLE_KEY = "serializable";
const std::string NON_SERIALIZABLE_KEY = "non-serializable";

struct TypeA : public ISerializableEntity
{
	TypeA(const Key& key) { SetKey(key); }
	void AggregateProtectedMember(SharedEntity obj) { AggregateMember(obj); };

	const SerializationMembersMap GetAggregatedProtectedMembers() { return GetAggregatedMembers(); };

	void Save(pt::ptree& tree, const std::string& path) const override {};
	void Load(pt::ptree& tree, const std::string& path) override {};
};

struct TypeB : public Entity 
{
	TypeB(const Key& key) { SetKey(key); }
};
} // end namespace

TEST(ISerializableEntity, CopyConstruct)
{
	TypeA serializable(ROOT_KEY);
	EXPECT_NO_THROW(TypeA copy(serializable));
}

TEST(ISerializableEntity, CopyAssign)
{
	TypeA serializable(ROOT_KEY);
	TypeA copy(ROOT_KEY);
	EXPECT_NO_THROW(copy = serializable);
}

TEST(ISerializableEntity, MoveConstruct)
{
	TypeA serializable(ROOT_KEY);
	EXPECT_NO_THROW(TypeA copy(std::move(serializable)));
}

TEST(ISerializableEntity, MoveAssign)
{
	TypeA serializable(ROOT_KEY);
	TypeA copy(ROOT_KEY);
	EXPECT_NO_THROW(copy = std::move(serializable));
}

TEST(ISerializableEntity, GetAggregatedMembers)
{
	TypeA obj = TypeA(ROOT_KEY);
	SharedEntity serializable = std::make_shared<TypeA>(SERIALIZABLE_KEY);
	SharedEntity nonSerializable = std::make_shared<TypeB>(NON_SERIALIZABLE_KEY);

	SerializationMembersMap entityMap = obj.GetAggregatedProtectedMembers();
	EXPECT_TRUE(entityMap.empty());

	obj.AggregateProtectedMember(serializable);
	obj.AggregateProtectedMember(nonSerializable);

	entityMap = obj.GetAggregatedProtectedMembers();

	EXPECT_TRUE(entityMap.size() == size_t(1));
	auto iter = entityMap.find(SERIALIZABLE_KEY);
	EXPECT_TRUE(iter != entityMap.end());
	iter = entityMap.find(NON_SERIALIZABLE_KEY);
	EXPECT_TRUE(iter == entityMap.end());
}