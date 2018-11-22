#include "gtest/gtest.h"

#include <memory>
#include <sstream>
#include <string>

#include "X/Entity.h"
#include "X/ISerializableEntity.h"

using global::ISerializableEntity;

namespace
{
const std::string KEY = "key";
const std::string TYPEA = "TypeA";
const std::string TYPEB = "TypeB";

class TypeB : public global::ISerializableEntity 
{
public:
	TypeB() {}
	std::string Serialize() const override { return TYPEB; };

	std::string deserializedTypeString;
	void Deserialize(const std::string& serialization) override
	{
		auto n = serialization.find(ISerializableEntity::GetDelimeter());
		deserializedTypeString = serialization.substr(0, n);
	};
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

	std::string Serialize() const { return data_; };
	void Deserialize(const std::string& serialization) override {};

	std::string data_ = TYPEA;
private:
	TypeB& GetTypeB() { return static_cast<TypeB&>(GetMember(KEY)); }
};
}

TEST(ISerializableEntity, SerializeNestedEntities)
{
	std::stringstream serialization;
	serialization << TypeA(KEY);
	std::string expected = TYPEA + ISerializableEntity::GetDelimeter() + TYPEB + ISerializableEntity::GetDelimeter();
	EXPECT_EQ(serialization.str(), expected);
}

TEST(ISerializableEntity, ThrowOnSerializeNestedEntities)
{
	TypeA ea;
	EXPECT_NO_THROW(ea.AddProtectedMember(KEY, std::make_unique<global::Entity>()));

	std::stringstream serialization;
	EXPECT_THROW(serialization << ea, std::runtime_error);
}

TEST(ISerializableEntity, SerializeEntity)
{
	std::stringstream serialization;
	serialization << TypeB();
	std::string expected = TYPEB + ISerializableEntity::GetDelimeter();
	EXPECT_EQ(serialization.str(), expected);
}

TEST(ISerializableEntity, DeserializeEntity)
{
	std::stringstream serialization;
	serialization << TypeB();
	TypeB b;
	EXPECT_TRUE(b.deserializedTypeString.empty());
	serialization >> b;
	EXPECT_EQ(b.deserializedTypeString, TYPEB);
}
