#include "gtest/gtest.h"

#include <memory>
#include <sstream>
#include <string>

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

// TODO
// enforce that all members of ISerializableEntity are ISerializable as well