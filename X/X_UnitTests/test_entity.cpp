#include "gtest/gtest.h"

#include <memory>
#include <sstream>
#include <string>

#include "X/Entity.h"
#include "X/Resource.h"

namespace
{
const std::string KEY = "key";
const std::string NEW_KEY = "newKeyB";
const int DEFAULT_INT_VALUE = 0;
const int INT_VALUE = 7;

class TypeB : public global::Entity 
{
public:
	TypeB() : resource_(DEFAULT_INT_VALUE) {}
	int& GetResource() { return resource_; }
	std::string Serialize() const { return typeid(TypeB).name();  };
private:
	int resource_;
};

class TypeA : public global::Entity
{
public:
	TypeA() { GetMembers().insert(std::make_pair(KEY, std::make_unique<TypeB>())); }
	TypeA(int d) : TypeA() { GetTypeB().GetResource() = d; }
	int GetValue() { return GetTypeB().GetResource(); }
	const Members& GetProtectedMembers() const { return GetMembers(); }
	Members& GetProtectedMembers() { return GetMembers(); }
	const MemberKeys GetProtectedMemberKeys() const { return GetMemberKeys(); }
	size_t GetExpectedMemberCount() { return 1; }
	std::string Serialize() const { return typeid(TypeA).name();  };
private:
	TypeB& GetTypeB() { return *std::static_pointer_cast<TypeB>(GetMembers()[KEY]); }
};
}

TEST(Entity, MoveConstruct)
{
	TypeA ea(INT_VALUE);

	// move
	TypeA eaMoved(std::move(ea));

	EXPECT_EQ(eaMoved.GetValue(), INT_VALUE);
}

TEST(Entity, MoveAssign)
{
	TypeA ea(INT_VALUE);

	TypeA eaMoved;
	EXPECT_EQ(eaMoved.GetValue(), DEFAULT_INT_VALUE);

	// move assign
	EXPECT_NO_THROW(eaMoved = std::move(ea));

	EXPECT_EQ(eaMoved.GetValue(), INT_VALUE);
}

TEST(Entity, CopyConstruct)
{
	TypeA ea(INT_VALUE);

	// copy
	TypeA eaCopied(ea);
	EXPECT_EQ(eaCopied.GetValue(), INT_VALUE);
}

TEST(Entity, CopyAssign)
{
	TypeA ea(INT_VALUE);
	TypeA eaCopied;
	EXPECT_EQ(eaCopied.GetValue(), DEFAULT_INT_VALUE);

	// copy assign
	EXPECT_NO_THROW(eaCopied = ea);

	EXPECT_EQ(eaCopied.GetValue(), INT_VALUE);
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
	TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMemberKeys());
	auto keys = ea.GetProtectedMemberKeys();
	EXPECT_EQ(keys.size(), ea.GetExpectedMemberCount());
	EXPECT_EQ(keys[0], KEY);
}

TEST(Entity, SerializeNestedEntities)
{
	std::stringstream serialization;
	serialization << TypeA();
	std::string expected = std::string(typeid(TypeA).name()) + typeid(TypeB).name();
	EXPECT_EQ(serialization.str(), expected);
}

TEST(Entity, SerializeEntity)
{
	std::stringstream serialization;
	serialization << TypeB();
	std::string expected = typeid(TypeB).name();
	EXPECT_EQ(serialization.str(), expected);
}