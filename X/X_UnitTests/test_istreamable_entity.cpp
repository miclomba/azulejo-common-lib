
#include "config.h"

#include <memory>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "X/Entity.h"
#include "X/IStreamableEntity.h"

using global::IStreamableEntity;

namespace
{
const std::string KEY = "key";
const std::string TYPEA = "TypeA";
const std::string TYPEB = "TypeB";
const char DELIM = ':';

class TypeB : public global::IStreamableEntity 
{
public:
	std::string ToString() const override { return TYPEB; };

	std::string unstreamedTypeString;
	void FromString(const std::string& str) override
	{
		auto n = str.find(IStreamableEntity::GetDelimeter());
		unstreamedTypeString = str.substr(0, n);
	};
};

class TypeA : public global::IStreamableEntity
{
public:
	TypeA(const std::string& key) { auto e = std::make_shared<TypeB>(); e->SetKey(key); AggregateMember(e); }
	TypeA() {}
	void AddProtectedMember(std::shared_ptr<Entity> entity) { AggregateMember(std::move(entity)); };
	const Members& GetProtectedMembers() { return GetAggregatedMembers(); };
	Entity& GetProtectedMember(const std::string& key) { return GetAggregatedMember(key); }
	const MemberKeys GetProtectedMemberKeys() const { return GetAggregatedMemberKeys(); }
	size_t GetExpectedMemberCount() { return 1; }

	std::string ToString() const { return data_; };
	void FromString(const std::string& str) override {};

	std::string data_ = TYPEA;
private:
	TypeB& GetTypeB() { return static_cast<TypeB&>(GetAggregatedMember(KEY)); }
};
}

TEST(IStreamableEntity, StreamNestedEntities)
{
	std::stringstream sstream;
	sstream << TypeA(KEY);
	std::string expected = TYPEA + IStreamableEntity::GetDelimeter() + TYPEB + IStreamableEntity::GetDelimeter();
	EXPECT_EQ(sstream.str(), expected);
}

TEST(IStreamableEntity, ThrowOnStreamNestedEntities)
{
	TypeA ea;
	auto e = std::make_shared<global::Entity>();
	e->SetKey(KEY);
	EXPECT_NO_THROW(ea.AddProtectedMember(e));

	std::stringstream sstream;
	EXPECT_THROW(sstream << ea, std::runtime_error);
}

TEST(IStreamableEntity, StreamEntity)
{
	std::stringstream sstream;
	sstream << TypeB();
	std::string expected = TYPEB + IStreamableEntity::GetDelimeter();
	EXPECT_EQ(sstream.str(), expected);
}

TEST(IStreamableEntity, UnstreamEntity)
{
	std::stringstream sstream;
	sstream << TypeB();
	TypeB b;
	EXPECT_TRUE(b.unstreamedTypeString.empty());
	sstream >> b;
	EXPECT_EQ(b.unstreamedTypeString, TYPEB);
}

TEST(IStreamableEntity, Delimiter)
{
	EXPECT_EQ(IStreamableEntity::GetDelimeter(), std::string(1,DELIM));
}
