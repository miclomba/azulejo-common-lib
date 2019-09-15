
#include "config.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Entities/Entity.h"

using entity::Entity;

using Key = entity::Entity::Key;
using SharedEntity = entity::Entity::SharedEntity;

namespace
{
const std::string KEY = "key";
const std::string MEMBER_KEY = "member_key";
const std::string NEW_KEY = "newKeyB";

class TypeB : public Entity 
{
public:
	TypeB() {}
	std::string Serialize() const { return typeid(TypeB).name();  };
};

class TypeA : public Entity
{
public:
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
	std::string Serialize() const { return typeid(TypeA).name(); };
private:
	TypeB& GetTypeB() { return static_cast<TypeB&>(*GetAggregatedMember(MEMBER_KEY)); }
};
} // end namespace anonymous

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

TEST(Entity, MoveConstruct)
{
	TypeA ea(KEY, MEMBER_KEY);
	Entity& memberEntity = *ea.GetProtectedMember(MEMBER_KEY);

	// move
	TypeA eaMoved(std::move(ea));

	entity::Entity& entityMoved = *eaMoved.GetProtectedMember(MEMBER_KEY);
	EXPECT_EQ(&memberEntity, &(*eaMoved.GetProtectedMember(MEMBER_KEY)));

	EXPECT_EQ(KEY, eaMoved.GetKey());
	EXPECT_EQ(MEMBER_KEY, entityMoved.GetKey());
}

TEST(Entity, MoveAssign)
{
	TypeA ea(KEY, MEMBER_KEY);
	Entity& memberEntity = *ea.GetProtectedMember(MEMBER_KEY);

	// move assign
	TypeA eaMoved;
	EXPECT_NO_THROW(eaMoved = std::move(ea));

	entity::Entity& entityMoved = *eaMoved.GetProtectedMember(MEMBER_KEY);
	EXPECT_EQ(&memberEntity, &(*eaMoved.GetProtectedMember(MEMBER_KEY)));

	EXPECT_EQ(KEY, eaMoved.GetKey());
	EXPECT_EQ(MEMBER_KEY, entityMoved.GetKey());
}

TEST(Entity, CopyConstruct)
{
	TypeA ea(KEY, MEMBER_KEY);

	// copy
	TypeA eaCopied(ea);
	EXPECT_EQ(ea.GetKey(), eaCopied.GetKey());
	EXPECT_NE(&ea.GetProtectedMember(MEMBER_KEY), &eaCopied.GetProtectedMember(MEMBER_KEY));
}

TEST(Entity, CopyAssign)
{
	TypeA ea(KEY, MEMBER_KEY);
	TypeA eaCopied;

	// copy assign
	EXPECT_NO_THROW(eaCopied = ea);

	EXPECT_EQ(ea.GetKey(), eaCopied.GetKey());
	EXPECT_NE(&ea.GetProtectedMember(MEMBER_KEY), &eaCopied.GetProtectedMember(MEMBER_KEY));
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