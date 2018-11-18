#include "gtest/gtest.h"

#include <memory>

#include "X/Entity.h"
#include "X/Resource.h"

using Members = global::Entity::Members;
using MemberKeys = global::Entity::MemberKeys;

namespace
{
const std::string KEY = "keyB";
const std::string NEW_KEY = "newKeyB";
const int DEFAULT_INT_VALUE = 0;
const int INT_VALUE = 7;

class IntResource : public global::Resource<int>
{
public:
	IntResource() {}
	IntResource(const int value)
	{
		EXPECT_NO_THROW(SetData(value));
		EXPECT_EQ(GetData(), value);
	}
	int GetData() const { return Data(); }
	void SetData(int value) { Data() = value; }
};

class EntityTypeB : public global::Entity 
{
public:
	EntityTypeB() { resource_.SetData(DEFAULT_INT_VALUE); }
	IntResource& GetResource() { return resource_; }
private:
	IntResource resource_;
};

class EntityTypeA : public global::Entity
{
public:
	EntityTypeA() { GetMembers().insert(std::make_pair(KEY, std::make_unique<EntityTypeB>())); }
	EntityTypeA(int d) : EntityTypeA() { GetEntityTypeB().GetResource().SetData(d); }
	int GetValue() { return GetEntityTypeB().GetResource().GetData(); }
	const Members& GetProtectedMembers() const { return GetMembers(); }
	Members& GetProtectedMembers() { return GetMembers(); }
	const MemberKeys GetProtectedMemberKeys() const { return GetMemberKeys(); }
	size_t GetExpectedMemberCount() { return 1; }
private:
	EntityTypeB& GetEntityTypeB() { return *std::static_pointer_cast<EntityTypeB>(GetMembers()[KEY]); }
};
}

TEST(Entity, MoveConstruct)
{
	EntityTypeA ea(INT_VALUE);

	// move
	EntityTypeA eaMoved(std::move(ea));

	EXPECT_EQ(eaMoved.GetValue(), INT_VALUE);
}

TEST(Entity, MoveAssign)
{
	EntityTypeA ea(INT_VALUE);

	EntityTypeA eaMoved;
	EXPECT_EQ(eaMoved.GetValue(), DEFAULT_INT_VALUE);

	// move assign
	EXPECT_NO_THROW(eaMoved = std::move(ea));

	EXPECT_EQ(eaMoved.GetValue(), INT_VALUE);
}

TEST(Entity, CopyConstruct)
{
	EntityTypeA ea(INT_VALUE);

	// copy
	EntityTypeA eaCopied(ea);
	EXPECT_EQ(eaCopied.GetValue(), INT_VALUE);
}

TEST(Entity, CopyAssign)
{
	EntityTypeA ea(INT_VALUE);
	EntityTypeA eaCopied;
	EXPECT_EQ(eaCopied.GetValue(), DEFAULT_INT_VALUE);

	// copy assign
	EXPECT_NO_THROW(eaCopied = ea);

	EXPECT_EQ(eaCopied.GetValue(), INT_VALUE);
}

TEST(Entity, GetMembers)
{
	EntityTypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	int count = ea.GetProtectedMembers().size();
	ea.GetProtectedMembers().insert(std::make_pair(NEW_KEY, std::make_unique<EntityTypeB>()));
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, GetMembersConst)
{
	const EntityTypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
}

TEST(Entity, GetMembersKeys)
{
	EntityTypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMemberKeys());
	auto keys = ea.GetProtectedMemberKeys();
	EXPECT_EQ(keys.size(), ea.GetExpectedMemberCount());
	EXPECT_EQ(keys[0], KEY);
}
