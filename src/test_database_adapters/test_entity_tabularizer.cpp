#include "test_database_adapters/config.h"

#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "Config/filesystem.hpp"

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/system/error_code.hpp>

#include "DatabaseAdapters/ITabularizableEntity.h"
#include "DatabaseAdapters/EntityDetabularizer.h"
#include "DatabaseAdapters/EntityTabularizer.h"
#include "DatabaseAdapters/ResourceTabularizer.h"
#include "DatabaseAdapters/Sqlite.h"
#include "Entities/Entity.h"

namespace pt = boost::property_tree;

using boost::system::error_code;
using database_adapters::EntityDetabularizer;
using database_adapters::EntityTabularizer;
using database_adapters::ITabularizableEntity;
using database_adapters::ResourceTabularizer;
using database_adapters::Sqlite;
using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
	const fs::path ROOT_DIR = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY;
	const std::string ENTITY_1A = "entity_1a";
	const std::string ENTITY_2A = "entity_2a";
	const std::string ENTITY_1B = "entity_1b";
	const std::string DB_NAME = "db.sqlite";
	const fs::path DB_PATH = ROOT_DIR / DB_NAME;
	const std::vector<std::string> ENTITY_KEYS{ENTITY_1A, ENTITY_2A, ENTITY_1B};
	const bool HAS_ROOT = true;
	const bool HAS_INTERMEDIATE = true;
	const bool HAS_LEAF = true;
	const std::string KEY = "key";
	const std::string VAL = "val";
	const std::string LOADED_SUFFIX = "_loaded";
	const std::string VAL_AFTER_LOAD = VAL + LOADED_SUFFIX;

	struct TabEntity : public ITabularizableEntity
	{
		TabEntity() = default;
		~TabEntity() = default;

		void AggregateMember(SharedEntity entity)
		{
			ITabularizableEntity::AggregateMember(std::move(entity));
		}

		std::shared_ptr<TabEntity> GetAggregateMember(const std::string &key)
		{
			return std::dynamic_pointer_cast<TabEntity>(ITabularizableEntity::GetAggregatedMember(key));
		}

		void Save(pt::ptree &tree, Sqlite &database) const override
		{
			tree.put(KEY, value_);
		}
		void Load(pt::ptree &tree, Sqlite &database) override
		{
			value_ = tree.get_child(KEY).data() + LOADED_SUFFIX;
		}

		std::string GetValue() const
		{
			return value_;
		}

	private:
		std::string value_ = VAL;
	};

	std::shared_ptr<TabEntity> CreateEntity(const Key &root, const std::string &intermediate, const Key &leaf)
	{
		auto rootEntity = std::make_shared<TabEntity>();
		auto leafEntity = std::make_shared<TabEntity>();
		auto intermediateEntity = std::make_shared<TabEntity>();

		rootEntity->SetKey(root);
		intermediateEntity->SetKey(intermediate);
		leafEntity->SetKey(leaf);

		intermediateEntity->AggregateMember(leafEntity);
		rootEntity->AggregateMember(intermediateEntity);

		return rootEntity;
	}

	void RegisterEntities(const Key &root, const std::string &intermediate, const Key &leaf)
	{
		EntityDetabularizer *detabularizer = EntityDetabularizer::GetInstance();
		detabularizer->GetRegistry().RegisterEntity<TabEntity>(root);
		detabularizer->GetRegistry().RegisterEntity<TabEntity>(intermediate);
		detabularizer->GetRegistry().RegisterEntity<TabEntity>(leaf);
	}

	struct SqliteRemover
	{
		SqliteRemover() { RemoveDB(); }
		~SqliteRemover() { RemoveDB(); }

		void RemoveDB()
		{
			if (fs::exists(DB_PATH))
			{
#if defined(__APPLE__) || defined(__MACH__)
				error_code ec;
				fs::permissions(DB_PATH, fs::perms::all_all, ec);
#else
				fs::permissions(DB_PATH, fs::perms::all, fs::perm_options::add);
#endif
				fs::remove(DB_PATH);
			}
		}
	};

	struct EntityTabularizerFixture
	{
		EntityTabularizerFixture(const bool hasRoot, const bool hasIntermediate, const bool hasLeaf)
		{
			entityMask_ = {hasRoot, hasIntermediate, hasLeaf};

			// validate tabularization
			EXPECT_FALSE(fs::exists(DB_PATH));

			entity_ = CreateEntity(ENTITY_KEYS[0], ENTITY_KEYS[1], ENTITY_KEYS[2]);
		}

		void VerifyTabularization(const std::string &key)
		{
			EXPECT_TRUE(fs::exists(DB_PATH));

			auto toLoadEntity = std::make_shared<TabEntity>();
			toLoadEntity->SetKey(key);

			RegisterEntities(ENTITY_KEYS[0], ENTITY_KEYS[1], ENTITY_KEYS[2]);

			EntityDetabularizer *detabularizer = EntityDetabularizer::GetInstance();
			detabularizer->OpenDatabase(DB_PATH);

			EXPECT_EQ(toLoadEntity->GetValue(), VAL);
			EXPECT_NO_THROW(detabularizer->LoadEntity(*toLoadEntity));
			EXPECT_EQ(toLoadEntity->GetValue(), VAL_AFTER_LOAD);

			if (entityMask_[0])
			{
				std::shared_ptr<TabEntity> child = toLoadEntity->GetAggregateMember(ENTITY_KEYS[1]);
				EXPECT_EQ(child->GetValue(), VAL_AFTER_LOAD);
				std::shared_ptr<TabEntity> grandChild = child->GetAggregateMember(ENTITY_KEYS[2]);
				EXPECT_EQ(grandChild->GetValue(), VAL_AFTER_LOAD);
			}
			else if (entityMask_[1])
			{
				std::shared_ptr<TabEntity> grandChild = toLoadEntity->GetAggregateMember(ENTITY_KEYS[2]);
				EXPECT_EQ(grandChild->GetValue(), VAL_AFTER_LOAD);
			}

			EntityDetabularizer::ResetInstance();
		}

		std::shared_ptr<TabEntity> GetEntity()
		{
			return entity_;
		}

		std::string GetRootKey() { return ENTITY_KEYS[0]; }
		std::string GetIntermKey() { return ENTITY_KEYS[1]; }
		std::string GetLeafKey() { return ENTITY_KEYS[2]; }

	private:
		SqliteRemover dbRemover_;

		std::array<bool, 3> entityMask_{false, false, false};
		std::shared_ptr<TabEntity> entity_;
	};
} // end namespace entity

TEST(EntityTabularizer, GetInstance)
{
	EXPECT_NO_THROW(EntityTabularizer::GetInstance());
	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();
	EXPECT_TRUE(tabularizer);
	EXPECT_NO_THROW(EntityTabularizer::ResetInstance());
}

TEST(EntityTabularizer, ResetInstance)
{
	EXPECT_NO_THROW(EntityTabularizer::ResetInstance());
}

TEST(EntityTabularizer, ResetInstanceClosesDatabase)
{
	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();
	tabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(EntityTabularizer::ResetInstance());

	tabularizer = EntityTabularizer::GetInstance();
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(EntityTabularizer::ResetInstance());
}

TEST(EntityTabularizer, GetDatabase)
{
	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();
	EXPECT_NO_THROW(tabularizer->GetDatabase());
	EntityTabularizer::ResetInstance();
}

TEST(EntityTabularizer, OpenDatabase)
{
	SqliteRemover remover;

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();

	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityTabularizer::ResetInstance();
}

TEST(EntityTabularizer, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();

	tabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(tabularizer->OpenDatabase(DB_PATH), std::runtime_error);

	EntityTabularizer::ResetInstance();
}

TEST(EntityTabularizer, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();

	tabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(tabularizer->CloseDatabase());
	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityTabularizer::ResetInstance();
}

TEST(EntityTabularizer, CloseDatabaseWhenDatabaseIsClosed)
{
	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();

	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	EntityTabularizer::ResetInstance();
}

TEST(EntityTabularizer, TabularizeFromRoot)
{
	EntityTabularizerFixture fixture(HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(tabularizer->Tabularize(*fixture.GetEntity()));
	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	fixture.VerifyTabularization(fixture.GetRootKey());

	EntityTabularizer::ResetInstance();
}

TEST(EntityTabularizer, TabularizeFromIntermediate)
{
	EntityTabularizerFixture fixture(!HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(tabularizer->Tabularize(*fixture.GetEntity()->GetAggregateMember(fixture.GetIntermKey())));
	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	fixture.VerifyTabularization(fixture.GetIntermKey());

	EntityTabularizer::ResetInstance();
}

TEST(EntityTabularizer, TabularizeFromLeaf)
{
	EntityTabularizerFixture fixture(!HAS_ROOT, !HAS_INTERMEDIATE, HAS_LEAF);

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	std::shared_ptr<TabEntity> intermEntity = fixture.GetEntity()->GetAggregateMember(fixture.GetIntermKey());
	EXPECT_NO_THROW(
		tabularizer->Tabularize(*intermEntity->GetAggregateMember(fixture.GetLeafKey())));
	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	fixture.VerifyTabularization(fixture.GetLeafKey());

	EntityTabularizer::ResetInstance();
}

TEST(EntityTabularizer, TabularizeWhenResourceTabularizerIsOpen)
{
	EntityTabularizerFixture fixture(HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	ResourceTabularizer *resourceTabularizer = ResourceTabularizer::GetInstance();
	resourceTabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(resourceTabularizer->GetDatabase().IsOpen());

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(tabularizer->Tabularize(*fixture.GetEntity()));
	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	EXPECT_TRUE(resourceTabularizer->GetDatabase().IsOpen());

	EntityTabularizer::ResetInstance();
	ResourceTabularizer::ResetInstance();
}

TEST(EntityTabularizer, TabularizeWhenResourceTabularizerIsClosed)
{
	EntityTabularizerFixture fixture(HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	ResourceTabularizer *resourceTabularizer = ResourceTabularizer::GetInstance();
	resourceTabularizer->CloseDatabase();
	EXPECT_FALSE(resourceTabularizer->GetDatabase().IsOpen());

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(tabularizer->Tabularize(*fixture.GetEntity()));
	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	EXPECT_FALSE(resourceTabularizer->GetDatabase().IsOpen());

	EntityTabularizer::ResetInstance();
	ResourceTabularizer::ResetInstance();
}

TEST(EntityTabularizer, TabularizeThrowsWhenGivenEntityWithoutKey)
{
	SqliteRemover dbRemover;

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));

	TabEntity entity;
	entity.SetKey("");
	EXPECT_THROW(tabularizer->Tabularize(entity), std::runtime_error);

	EntityTabularizer::ResetInstance();
}

TEST(EntityTabularizer, TabularizeThrowsIfDatabaseIsNotOpen)
{
	SqliteRemover dbRemover;

	EntityTabularizer *tabularizer = EntityTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	TabEntity entity;
	entity.SetKey(ENTITY_1A);
	EXPECT_THROW(tabularizer->Tabularize(entity), std::runtime_error);

	EntityTabularizer::ResetInstance();
}
