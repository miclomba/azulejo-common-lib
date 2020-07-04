
#include "config.h"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Entities/Entity.h"
#include "Entities/IStreamableEntity.h"

using entity::Entity;
using entity::IStreamableEntity;

namespace
{
const std::string KEY = "key";
const std::string TYPEA = "TypeA";
const std::string TYPEB = "TypeB";
const char DELIM = ':';

struct TypeB : public IStreamableEntity 
{
	std::string ToString() const override { return TYPEB; };
	void FromString(const std::string& str) override
	{
		auto n = str.find(IStreamableEntity::GetDelimeter());
		unstreamedTypeString = str.substr(0, n);
	};

	std::string GetUnstreamedType() const { return unstreamedTypeString; };
private:
	std::string unstreamedTypeString;
};

struct TypeA : public IStreamableEntity
{
	TypeA(const std::string& key) 
	{ 
		auto e = std::make_shared<TypeB>(); 
		e->SetKey(key); 
		AggregateMember(e); 
	}
	TypeA() {}

	std::string ToString() const override { return TYPEA; };
	void FromString(const std::string& str) override {};

	void AddProtectedMember(SharedEntity entity) { AggregateMember(std::move(entity)); };
};

struct TypeC : public Entity {};
} // end namespace anonymous

TEST(IStreamableEntity, Construct)
{
	EXPECT_NO_THROW(TypeA streamable);
}

TEST(IStreamableEntity, CopyConstruct)
{
	TypeA source;
	EXPECT_NO_THROW(TypeA target(source));
}

TEST(IStreamableEntity, CopyAssign)
{
	TypeA source;
	TypeA target;
	EXPECT_NO_THROW(target = source);
}

TEST(IStreamableEntity, MoveConstruct)
{
	TypeA source;
	EXPECT_NO_THROW(TypeA target(std::move(source)));
}

TEST(IStreamableEntity, MoveAssign)
{
	TypeA source;
	TypeA target;
	EXPECT_NO_THROW(target = std::move(source));
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
	auto e = std::make_shared<TypeC>();
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
	EXPECT_TRUE(b.GetUnstreamedType().empty());
	sstream >> b;
	EXPECT_EQ(b.GetUnstreamedType(), TYPEB);
}

TEST(IStreamableEntity, GetDelimiter)
{
	EXPECT_EQ(IStreamableEntity::GetDelimeter(), std::string(1,DELIM));
}
