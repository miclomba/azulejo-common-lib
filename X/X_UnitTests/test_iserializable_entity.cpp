#include "gtest/gtest.h"

#include <memory>
#include <sstream>
#include <string>

#include "X/Entity.h"
#include "X/ISerializableEntity.h"

namespace
{
const std::string KEY = "key";

class TypeB : public global::ISerializableEntity 
{
public:
	TypeB() {}
	std::string Serialize() const { return typeid(TypeB).name();  };
};

class TypeA : public global::ISerializableEntity
{
public:
	TypeA(const std::string& key) { AggregateMember(key, std::make_unique<TypeB>()); }
	TypeA() {}
	void AddProtectedMember(const std::string& key, std::shared_ptr<Entity> entity) { AggregateMember(key, std::move(entity)); };
	const Members& GetProtectedMembers() { return GetMembers(); };
	Entity& GetProtectedMember(const std::string& key) { return GetMember(key); }
	const MemberKeys GetProtectedMemberKeys() const { return GetMemberKeys(); }
	size_t GetExpectedMemberCount() { return 1; }

	std::string Serialize() const { return typeid(TypeA).name(); };
private:
	TypeB& GetTypeB() { return static_cast<TypeB&>(GetMember(KEY)); }
};
}

TEST(ISerializableEntity, SerializeNestedEntities)
{
	std::stringstream serialization;
	serialization << TypeA(KEY);
	std::string expected = std::string(typeid(TypeA).name()) + typeid(TypeB).name();
	EXPECT_EQ(serialization.str(), expected);
}

TEST(ISerializableEntity, SerializeEntity)
{
	std::stringstream serialization;
	serialization << TypeB();
	std::string expected = typeid(TypeB).name();
	EXPECT_EQ(serialization.str(), expected);
}

TEST(ISerializableEntity, AddMember)
{
	TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = ea.GetProtectedMembers().size();
	ea.AddProtectedMember(KEY, std::make_unique<TypeA>());
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(ISerializableEntity, ThrowOnAddMember)
{
	TypeA ea;
	EXPECT_THROW(ea.AddProtectedMember(KEY, std::make_unique<global::Entity>()), std::runtime_error);
}

// TODO
// enforce that all members of ISerializableEntity are ISerializable as well