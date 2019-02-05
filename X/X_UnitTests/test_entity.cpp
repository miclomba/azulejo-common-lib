
#include "config.h"

#include <memory>
#include <string>
#include <gtest/gtest.h>

#include "X/Entity.h"

namespace
{
const std::string KEY = "key";
const std::string NEW_KEY = "newKeyB";

class TypeB : public global::Entity 
{
public:
	TypeB() {}
	std::string Serialize() const { return typeid(TypeB).name();  };
};

class TypeA : public global::Entity
{
public:
	TypeA(const std::string& key) { auto e = std::make_shared<TypeB>(); e->SetKey(key); AggregateMember(e); }
	TypeA() {}
	void AddProtectedMember(std::shared_ptr<Entity> entity) { AggregateMember(std::move(entity)); };
	void AddProtectedMember(const std::string& key, std::shared_ptr<Entity> entity) { AggregateMember(key, std::move(entity)); };
	const Members& GetProtectedMembers() { return GetAggregatedMembers(); };
	Entity& GetProtectedMember(const std::string& key) { return *GetAggregatedMember(key); }
	const MemberKeys GetProtectedMemberKeys() const { return GetAggregatedMemberKeys(); }
	size_t GetExpectedMemberCount() { return 1; }
	std::string Serialize() const { return typeid(TypeA).name(); };
private:
	TypeB& GetTypeB() { return static_cast<TypeB&>(*GetAggregatedMember(KEY)); }
};
}

TEST(Entity, MoveConstruct)
{
	TypeA ea(KEY);
	auto& entity = ea.GetProtectedMember(KEY);
	// move
	TypeA eaMoved(std::move(ea));

	EXPECT_EQ(&entity, &eaMoved.GetProtectedMember(KEY));
}

TEST(Entity, MoveAssign)
{
	TypeA ea(KEY);
	auto& entity = ea.GetProtectedMember(KEY);

	TypeA eaMoved;

	// move assign
	EXPECT_NO_THROW(eaMoved = std::move(ea));
	EXPECT_EQ(&entity, &eaMoved.GetProtectedMember(KEY));
}

TEST(Entity, CopyConstruct)
{
	TypeA ea(KEY);

	// copy
	TypeA eaCopied(ea);
	EXPECT_NE(&ea.GetProtectedMember(KEY), &eaCopied.GetProtectedMember(KEY));
}

TEST(Entity, CopyAssign)
{
	TypeA ea(KEY);
	TypeA eaCopied;

	// copy assign
	EXPECT_NO_THROW(eaCopied = ea);
	EXPECT_NE(&ea.GetProtectedMember(KEY), &eaCopied.GetProtectedMember(KEY));
}

TEST(Entity, AddMember)
{
	TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = ea.GetProtectedMembers().size();
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, AddMemberWithKey)
{
	TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = ea.GetProtectedMembers().size();
	auto e = std::make_shared<TypeB>();
	ea.AddProtectedMember(NEW_KEY, e);
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, ThrowOnAddMember)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);

	auto e2 = std::make_shared<TypeB>();
	e2->SetKey(NEW_KEY);
	EXPECT_THROW(ea.AddProtectedMember(e2), std::runtime_error);
}

TEST(Entity, ThrowOnAddNullptrMember)
{
	TypeA ea;
	EXPECT_THROW(ea.AddProtectedMember(nullptr), std::runtime_error);
}

TEST(Entity, GetMember)
{
	TypeA ea(KEY);
	EXPECT_NO_THROW(ea.GetProtectedMember(KEY));
}

TEST(Entity, ThrowOnGetMember)
{
	TypeA ea;
	EXPECT_THROW(ea.GetProtectedMember(KEY), std::runtime_error);
}

TEST(Entity, GetMembers)
{
	TypeA ea(KEY);
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = 1;
	EXPECT_EQ(count, ea.GetProtectedMembers().size());
}

TEST(Entity, GetMembersKeys)
{
	TypeA ea(KEY);
	EXPECT_NO_THROW(ea.GetProtectedMemberKeys());
	auto keys = ea.GetProtectedMemberKeys();
	EXPECT_EQ(keys.size(), ea.GetExpectedMemberCount());
	EXPECT_EQ(keys[0], KEY);
}