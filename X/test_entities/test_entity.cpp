
#include "config.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Entities/Entity.h"

using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
const std::string KEY = "key";
const std::string MEMBER_KEY = "member_key";
const std::string NEW_KEY = "newKeyB";

struct TypeB : public Entity 
{
	TypeB() {}
};

struct TypeA : public Entity
{
	TypeA(const Key& key, const Key& memberKey) 
	{ 
		SetKey(key);
		auto e = std::make_shared<TypeB>(); 
		e->SetKey(memberKey); 
		AggregateMember(e); 
	}
	TypeA() {}
	
	void AddProtectedMember(SharedEntity entity) { AggregateMember(std::move(entity)); };
	void AddProtectedMember(const Key& key) { AggregateMember(key); };

	const std::map<Key, SharedEntity>& GetProtectedMembers() const { return GetAggregatedMembers(); };
	std::map<Key, SharedEntity>& GetProtectedMembers() { return GetAggregatedMembers(); };

	SharedEntity GetProtectedMember(const Key& key) const { return GetAggregatedMember(key); }
	const std::vector<Key> GetProtectedMemberKeys() const { return GetAggregatedMemberKeys(); }
	size_t GetExpectedMemberCount() const { return 1; }
};
} // end namespace anonymous

TEST(Entity, Construct)
{
	EXPECT_NO_THROW(TypeB entity);
}

TEST(Entity, MoveConstruct)
{
	TypeA source(KEY, MEMBER_KEY);
	Entity& memberEntity = *source.GetProtectedMember(MEMBER_KEY);

	// move
	TypeA target(std::move(source));

	Entity& entityMoved = *target.GetProtectedMember(MEMBER_KEY);
	EXPECT_EQ(&memberEntity, &(*target.GetProtectedMember(MEMBER_KEY)));

	EXPECT_EQ(KEY, target.GetKey());
	EXPECT_EQ(MEMBER_KEY, entityMoved.GetKey());
}

TEST(Entity, MoveAssign)
{
	TypeA source(KEY, MEMBER_KEY);
	Entity& memberEntity = *source.GetProtectedMember(MEMBER_KEY);

	// move assign
	TypeA target;
	EXPECT_NO_THROW(target = std::move(source));

	Entity& entityMoved = *target.GetProtectedMember(MEMBER_KEY);
	EXPECT_EQ(&memberEntity, &(*target.GetProtectedMember(MEMBER_KEY)));

	EXPECT_EQ(KEY, target.GetKey());
	EXPECT_EQ(MEMBER_KEY, entityMoved.GetKey());
}

TEST(Entity, CopyConstruct)
{
	TypeA source(KEY, MEMBER_KEY);

	// copy
	TypeA target(source);
	EXPECT_EQ(source.GetKey(), target.GetKey());
	EXPECT_NE(&source.GetProtectedMember(MEMBER_KEY), &target.GetProtectedMember(MEMBER_KEY));
}

TEST(Entity, CopyAssign)
{
	TypeA source(KEY, MEMBER_KEY);
	TypeA target;

	// copy assign
	EXPECT_NO_THROW(target = source);

	EXPECT_EQ(source.GetKey(), target.GetKey());
	EXPECT_NE(&source.GetProtectedMember(MEMBER_KEY), &target.GetProtectedMember(MEMBER_KEY));
}

TEST(Entity, GetKey)
{
	TypeA obj(KEY, MEMBER_KEY);
	Key key = obj.GetKey();
	EXPECT_EQ(key, KEY);
}

TEST(Entity, GetKeyFromConst)
{
	const TypeA obj(KEY, MEMBER_KEY);
	Key key = obj.GetKey();
	EXPECT_EQ(key, KEY);
}

TEST(Entity, SetKey)
{
	TypeA obj(KEY, MEMBER_KEY);
	obj.SetKey(MEMBER_KEY);
	Key key = obj.GetKey();
	EXPECT_EQ(key, MEMBER_KEY);
}

TEST(Entity, AggregateMember)
{
	TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = ea.GetProtectedMembers().size();
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, AggregateMemberKey)
{
	TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = ea.GetProtectedMembers().size();
	ea.AddProtectedMember(NEW_KEY);
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, AggregateMemberThrowsWhenUsingExistingKeyCase1)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);

	auto e2 = std::make_shared<TypeB>();
	e2->SetKey(NEW_KEY);
	EXPECT_THROW(ea.AddProtectedMember(e2), std::runtime_error);
}

TEST(Entity, AggregateMemberThrowsWhenUsingExistingKeyCase2)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);

	EXPECT_THROW(ea.AddProtectedMember(NEW_KEY), std::runtime_error);
}

TEST(Entity, AggregateMemberThrowsWhenGivenNullptr)
{
	TypeA ea;
	EXPECT_THROW(ea.AddProtectedMember(std::shared_ptr<TypeA>()), std::runtime_error);
}

TEST(Entity, GetAggregatedMember)
{
	TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMember(MEMBER_KEY));
	SharedEntity member = ea.GetProtectedMember(MEMBER_KEY);
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);
}

TEST(Entity, GetAggregatedMemberFromConst)
{
	const TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMember(MEMBER_KEY));
	SharedEntity member = ea.GetProtectedMember(MEMBER_KEY);
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);
}

TEST(Entity, GetAggregatedMemberThrows)
{
	TypeA ea;
	EXPECT_THROW(ea.GetProtectedMember(MEMBER_KEY), std::runtime_error);
}

TEST(Entity, GetAggregatedMembers)
{
	TypeA ea(KEY, MEMBER_KEY);

	EXPECT_NO_THROW(ea.GetProtectedMembers());
	std::map<Key, SharedEntity>& members = ea.GetProtectedMembers();
	
	auto iter = members.find(MEMBER_KEY);
	SharedEntity member = iter->second;

	EXPECT_EQ(ea.GetExpectedMemberCount(), members.size());
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);
}

TEST(Entity, GetAggregatedMembersFromConst)
{
	const TypeA ea(KEY, MEMBER_KEY);

	EXPECT_NO_THROW(ea.GetProtectedMembers());
	const std::map<Key, SharedEntity>& members = ea.GetProtectedMembers();

	auto iter = members.find(MEMBER_KEY);
	SharedEntity member = iter->second;

	EXPECT_EQ(ea.GetExpectedMemberCount(), members.size());
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);
}

TEST(Entity, GetAggregatedMembersKeys)
{
	TypeA ea(KEY, MEMBER_KEY);

	EXPECT_NO_THROW(ea.GetProtectedMemberKeys());
	const std::vector<Key> keys = ea.GetProtectedMemberKeys();

	EXPECT_EQ(ea.GetExpectedMemberCount(), keys.size());
	EXPECT_GT(ea.GetExpectedMemberCount(), size_t(0));
	EXPECT_EQ(keys[0], MEMBER_KEY);
}

TEST(Entity, GetAggregatedMembersKeysFromConst)
{
	const TypeA ea(KEY, MEMBER_KEY);

	EXPECT_NO_THROW(ea.GetProtectedMemberKeys());
	const std::vector<Key> keys = ea.GetProtectedMemberKeys();

	EXPECT_EQ(ea.GetExpectedMemberCount(), keys.size());
	EXPECT_GT(ea.GetExpectedMemberCount(), size_t(0));
	EXPECT_EQ(keys[0], MEMBER_KEY);
}