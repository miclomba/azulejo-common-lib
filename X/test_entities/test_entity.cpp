
#include "config.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Entities/Entity.h"

namespace
{
const std::string KEY = "key";
const std::string MEMBER_KEY = "member_key";
const std::string NEW_KEY = "newKeyB";

class TypeB : public entity::Entity 
{
public:
	TypeB() {}
	std::string Serialize() const { return typeid(TypeB).name();  };
};

class TypeA : public entity::Entity
{
public:
	TypeA(const std::string& key, const std::string& memberKey) 
	{ 
		SetKey(key);
		auto e = std::make_shared<TypeB>(); 
		e->SetKey(memberKey); 
		AggregateMember(e); 
	}
	TypeA() {}
	void AddProtectedMember(std::shared_ptr<Entity> entity) { AggregateMember(std::move(entity)); };
	void AddProtectedMember(const std::string& key) { AggregateMember(key); };
	const std::map<std::string, std::shared_ptr<Entity>>& GetProtectedMembers() { return GetAggregatedMembers(); };
	Entity& GetProtectedMember(const std::string& key) { return *GetAggregatedMember(key); }
	const std::vector<std::string> GetProtectedMemberKeys() const { return GetAggregatedMemberKeys(); }
	size_t GetExpectedMemberCount() { return 1; }
	std::string Serialize() const { return typeid(TypeA).name(); };
private:
	TypeB& GetTypeB() { return static_cast<TypeB&>(*GetAggregatedMember(MEMBER_KEY)); }
};
} // end namespace anonymous

TEST(Entity, MoveConstruct)
{
	TypeA ea(KEY, MEMBER_KEY);
	entity::Entity& entity = ea.GetProtectedMember(MEMBER_KEY);
	// move
	TypeA eaMoved(std::move(ea));

	EXPECT_EQ(&entity, &eaMoved.GetProtectedMember(MEMBER_KEY));
}

TEST(Entity, MoveAssign)
{
	TypeA ea(KEY, MEMBER_KEY);
	entity::Entity& entity = ea.GetProtectedMember(MEMBER_KEY);

	TypeA eaMoved;

	// move assign
	EXPECT_NO_THROW(eaMoved = std::move(ea));
	EXPECT_EQ(&entity, &eaMoved.GetProtectedMember(MEMBER_KEY));
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
	ea.AddProtectedMember(NEW_KEY);
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, ThrowOnAddMemberUsingExistingKeyCase1)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);

	auto e2 = std::make_shared<TypeB>();
	e2->SetKey(NEW_KEY);
	EXPECT_THROW(ea.AddProtectedMember(e2), std::runtime_error);
}

TEST(Entity, ThrowOnAddMemberUsingExistingKeyCase2)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);

	EXPECT_THROW(ea.AddProtectedMember(NEW_KEY), std::runtime_error);
}

TEST(Entity, ThrowOnAddMemberNullptr)
{
	TypeA ea;
	EXPECT_THROW(ea.AddProtectedMember(std::shared_ptr<TypeA>()), std::runtime_error);
}

TEST(Entity, GetMember)
{
	TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMember(MEMBER_KEY));
}

TEST(Entity, ThrowOnGetMember)
{
	TypeA ea;
	EXPECT_THROW(ea.GetProtectedMember(MEMBER_KEY), std::runtime_error);
}

TEST(Entity, GetMembers)
{
	TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = 1;
	EXPECT_EQ(count, ea.GetProtectedMembers().size());
}

TEST(Entity, GetMembersKeys)
{
	TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMemberKeys());
	const std::vector<std::string> keys = ea.GetProtectedMemberKeys();
	EXPECT_EQ(keys.size(), ea.GetExpectedMemberCount());
	EXPECT_EQ(keys[0], MEMBER_KEY);
}