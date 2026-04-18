#include "test_database_adapters/config.h"

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

#include "DatabaseAdapters/IPersistableEntity.h"
#include "DatabaseAdapters/Sqlite.h"

namespace pt = boost::property_tree;

using database_adapters::IPersistableEntity;
using database_adapters::Sqlite;
using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
	const std::string ROOT_KEY = "root";
	const std::string LOADABLE_KEY = "persistable";
	const std::string NON_LOADABLE_KEY = "non-persistable";

	struct TypeA : public IPersistableEntity
	{
		TypeA() = default;
		TypeA(const Key &key) { SetKey(key); }
		void AggregateProtectedMember(SharedEntity obj) { AggregateMember(obj); };

		void Save(pt::ptree &tree, Sqlite &db) const override {}
		void Load(pt::ptree &tree, Sqlite &db) override {}
	};
} // end namespace

TEST(IPersistableEntity, Construct)
{
	EXPECT_NO_THROW(TypeA persistable);
}

TEST(IPersistableEntity, CopyConstruct)
{
	TypeA source(ROOT_KEY);
	EXPECT_NO_THROW(TypeA target(source));
}

TEST(IPersistableEntity, CopyAssign)
{
	TypeA source(ROOT_KEY);
	TypeA target(ROOT_KEY);
	EXPECT_NO_THROW(target = source);
}

TEST(IPersistableEntity, MoveConstruct)
{
	TypeA source(ROOT_KEY);
	EXPECT_NO_THROW(TypeA target(std::move(source)));
}

TEST(IPersistableEntity, MoveAssign)
{
	TypeA source(ROOT_KEY);
	TypeA target(ROOT_KEY);
	EXPECT_NO_THROW(target = std::move(source));
}
