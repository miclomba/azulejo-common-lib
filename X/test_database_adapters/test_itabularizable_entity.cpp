#include "config.h"

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

#include "DatabaseAdapters/ITabularizableEntity.h"
#include "DatabaseAdapters/Sqlite.h"

namespace pt = boost::property_tree;

using database_adapters::ITabularizableEntity;
using database_adapters::Sqlite;
using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
const std::string ROOT_KEY = "root";
const std::string TABULARIZABLE_KEY = "tabularizable";
const std::string NON_TABULARIZABLE_KEY = "non-tabularizable";

struct TypeA : public ITabularizableEntity
{
	TypeA() = default;
	TypeA(const Key& key) { SetKey(key); }
	void AggregateProtectedMember(SharedEntity obj) { AggregateMember(obj); };

	void Save(Sqlite& tree) const override {}
	void Load(Sqlite& tree) override {}
};
} // end namespace

TEST(ITabularizableEntity, Construct)
{
	EXPECT_NO_THROW(TypeA tabularizable());
}

TEST(ITabularizableEntity, CopyConstruct)
{
	TypeA source(ROOT_KEY);
	EXPECT_NO_THROW(TypeA target(source));
}

TEST(ITabularizableEntity, CopyAssign)
{
	TypeA source(ROOT_KEY);
	TypeA target(ROOT_KEY);
	EXPECT_NO_THROW(target = source);
}

TEST(ITabularizableEntity, MoveConstruct)
{
	TypeA source(ROOT_KEY);
	EXPECT_NO_THROW(TypeA target(std::move(source)));
}

TEST(ITabularizableEntity, MoveAssign)
{
	TypeA source(ROOT_KEY);
	TypeA target(ROOT_KEY);
	EXPECT_NO_THROW(target = std::move(source));
}
