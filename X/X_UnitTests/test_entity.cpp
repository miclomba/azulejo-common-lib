#include "gtest/gtest.h"

#include <memory>
#include <string>

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
	TypeA(const std::string& key) { AggregateMember(key, std::make_unique<TypeB>()); }
	TypeA() {}
	void AddProtectedMember(const std::string& key, std::shared_ptr<Entity> entity) { AggregateMember(key, std::move(entity)); };
	const Members& GetProtectedMembers() { return GetAggregatedMembers(); };
	Entity& GetProtectedMember(const std::string& key) { return GetAggregatedMember(key); }
	const MemberKeys GetProtectedMemberKeys() const { return GetAggregatedMemberKeys(); }
	size_t GetExpectedMemberCount() { return 1; }
	std::string Serialize() const { return typeid(TypeA).name(); };
private:
	TypeB& GetTypeB() { return static_cast<TypeB&>(GetAggregatedMember(KEY)); }
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
	ea.AddProtectedMember(NEW_KEY, std::make_unique<TypeB>());
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, ThrowOnAddMember)
{
	TypeA ea;
	ea.AddProtectedMember(NEW_KEY, std::make_unique<TypeB>());
	EXPECT_THROW(ea.AddProtectedMember(NEW_KEY, std::make_unique<TypeB>()), std::runtime_error);
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