#include "config.h"

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

#include "FilesystemAdapters/ISerializableEntity.h"

namespace pt = boost::property_tree;

using entity::Entity;
using filesystem_adapters::ISerializableEntity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
const std::string ROOT_KEY = "root";
const std::string SERIALIZABLE_KEY = "serializable";
const std::string NON_SERIALIZABLE_KEY = "non-serializable";

struct TypeA : public ISerializableEntity
{
	TypeA() = default;
	TypeA(const Key& key) { SetKey(key); }
	void AggregateProtectedMember(SharedEntity obj) { AggregateMember(obj); };

	void Save(pt::ptree& tree, const std::string& path) const override {};
	void Load(pt::ptree& tree, const std::string& path) override {};
};

struct TypeB : public Entity 
{
	TypeB(const Key& key) { SetKey(key); }
};
} // end namespace

TEST(ISerializableEntity, Construct)
{
	EXPECT_NO_THROW(TypeA serializable());
}

TEST(ISerializableEntity, CopyConstruct)
{
	TypeA source(ROOT_KEY);
	EXPECT_NO_THROW(TypeA target(source));
}

TEST(ISerializableEntity, CopyAssign)
{
	TypeA source(ROOT_KEY);
	TypeA target(ROOT_KEY);
	EXPECT_NO_THROW(target = source);
}

TEST(ISerializableEntity, MoveConstruct)
{
	TypeA source(ROOT_KEY);
	EXPECT_NO_THROW(TypeA target(std::move(source)));
}

TEST(ISerializableEntity, MoveAssign)
{
	TypeA source(ROOT_KEY);
	TypeA target(ROOT_KEY);
	EXPECT_NO_THROW(target = std::move(source));
}
