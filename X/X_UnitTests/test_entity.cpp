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
	TypeA(const std::string& key) { GetMembers().insert(std::make_pair(key, std::make_unique<TypeB>())); }
	TypeA() {}
	const Members& GetProtectedMembers() const { return GetMembers(); }
	Members& GetProtectedMembers() { return GetMembers(); }
	const MemberKeys GetProtectedMemberKeys() const { return GetMemberKeys(); }
	size_t GetExpectedMemberCount() { return 1; }
	std::string Serialize() const { return typeid(TypeA).name(); };
private:
	TypeB& GetTypeB() { return *std::static_pointer_cast<TypeB>(GetMembers()[KEY]); }
};
}

TEST(Entity, MoveConstruct)
{
	TypeA ea(KEY);
	auto memberAddress = ea.GetProtectedMembers()[KEY].get();
	// move
	TypeA eaMoved(std::move(ea));

	EXPECT_EQ(memberAddress, eaMoved.GetProtectedMembers()[KEY].get());
}

TEST(Entity, MoveAssign)
{
	TypeA ea(KEY);
	auto memberAddress = ea.GetProtectedMembers()[KEY].get();

	TypeA eaMoved;

	// move assign
	EXPECT_NO_THROW(eaMoved = std::move(ea));
	EXPECT_EQ(memberAddress, eaMoved.GetProtectedMembers()[KEY].get());
}

TEST(Entity, CopyConstruct)
{
	TypeA ea(KEY);

	// copy
	TypeA eaCopied(ea);
	EXPECT_NE(ea.GetProtectedMembers()[KEY].get(), eaCopied.GetProtectedMembers()[KEY].get());
}

TEST(Entity, CopyAssign)
{
	TypeA ea(KEY);
	TypeA eaCopied;

	// copy assign
	EXPECT_NO_THROW(eaCopied = ea);
	EXPECT_NE(ea.GetProtectedMembers()[KEY].get(), eaCopied.GetProtectedMembers()[KEY].get());
}

TEST(Entity, GetMembers)
{
	TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = ea.GetProtectedMembers().size();
	ea.GetProtectedMembers().insert(std::make_pair(NEW_KEY, std::make_unique<TypeB>()));
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, GetMembersConst)
{
	const TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
}

TEST(Entity, GetMembersKeys)
{
	TypeA ea(KEY);
	EXPECT_NO_THROW(ea.GetProtectedMemberKeys());
	auto keys = ea.GetProtectedMemberKeys();
	EXPECT_EQ(keys.size(), ea.GetExpectedMemberCount());
	EXPECT_EQ(keys[0], KEY);
}