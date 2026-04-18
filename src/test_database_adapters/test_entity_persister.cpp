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

#include "DatabaseAdapters/IPersistableEntity.h"
#include "DatabaseAdapters/EntityLoader.h"
#include "DatabaseAdapters/EntityPersister.h"
#include "DatabaseAdapters/ResourcePersister.h"
#include "DatabaseAdapters/Sqlite.h"
#include "Entities/Entity.h"

namespace pt = boost::property_tree;

using boost::system::error_code;
using database_adapters::EntityLoader;
using database_adapters::EntityPersister;
using database_adapters::IPersistableEntity;
using database_adapters::ResourcePersister;
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

	struct TabEntity : public IPersistableEntity
	{
		TabEntity() = default;
		~TabEntity() = default;

		void AggregateMember(SharedEntity entity)
		{
			IPersistableEntity::AggregateMember(std::move(entity));
		}

		std::shared_ptr<TabEntity> GetAggregateMember(const std::string &key)
		{
			return std::dynamic_pointer_cast<TabEntity>(IPersistableEntity::GetAggregatedMember(key));
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
		EntityLoader *loader = EntityLoader::GetInstance();
		loader->GetRegistry().RegisterEntity<TabEntity>(root);
		loader->GetRegistry().RegisterEntity<TabEntity>(intermediate);
		loader->GetRegistry().RegisterEntity<TabEntity>(leaf);
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

	struct EntityPersisterFixture
	{
		EntityPersisterFixture(const bool hasRoot, const bool hasIntermediate, const bool hasLeaf)
		{
			entityMask_ = {hasRoot, hasIntermediate, hasLeaf};

			// validate persistence
			EXPECT_FALSE(fs::exists(DB_PATH));

			entity_ = CreateEntity(ENTITY_KEYS[0], ENTITY_KEYS[1], ENTITY_KEYS[2]);
		}

		void VerifyPersistence(const std::string &key)
		{
			EXPECT_TRUE(fs::exists(DB_PATH));

			auto toLoadEntity = std::make_shared<TabEntity>();
			toLoadEntity->SetKey(key);

			RegisterEntities(ENTITY_KEYS[0], ENTITY_KEYS[1], ENTITY_KEYS[2]);

			EntityLoader *loader = EntityLoader::GetInstance();
			loader->OpenDatabase(DB_PATH);

			EXPECT_EQ(toLoadEntity->GetValue(), VAL);
			EXPECT_NO_THROW(loader->LoadEntity(*toLoadEntity));
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

			EntityLoader::ResetInstance();
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

TEST(EntityPersister, GetInstance)
{
	EXPECT_NO_THROW(EntityPersister::GetInstance());
	EntityPersister *persister = EntityPersister::GetInstance();
	EXPECT_TRUE(persister);
	EXPECT_NO_THROW(EntityPersister::ResetInstance());
}

TEST(EntityPersister, ResetInstance)
{
	EXPECT_NO_THROW(EntityPersister::ResetInstance());
}

TEST(EntityPersister, ResetInstanceClosesDatabase)
{
	EntityPersister *persister = EntityPersister::GetInstance();
	persister->OpenDatabase(DB_PATH);
	EXPECT_TRUE(persister->GetDatabase().IsOpen());
	EXPECT_NO_THROW(EntityPersister::ResetInstance());

	persister = EntityPersister::GetInstance();
	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(EntityPersister::ResetInstance());
}

TEST(EntityPersister, GetDatabase)
{
	EntityPersister *persister = EntityPersister::GetInstance();
	EXPECT_NO_THROW(persister->GetDatabase());
	EntityPersister::ResetInstance();
}

TEST(EntityPersister, OpenDatabase)
{
	SqliteRemover remover;

	EntityPersister *persister = EntityPersister::GetInstance();

	EXPECT_FALSE(persister->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	EXPECT_TRUE(persister->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityPersister::ResetInstance();
}

TEST(EntityPersister, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	EntityPersister *persister = EntityPersister::GetInstance();

	persister->OpenDatabase(DB_PATH);
	EXPECT_TRUE(persister->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(persister->OpenDatabase(DB_PATH), std::runtime_error);

	EntityPersister::ResetInstance();
}

TEST(EntityPersister, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	EntityPersister *persister = EntityPersister::GetInstance();

	persister->OpenDatabase(DB_PATH);
	EXPECT_TRUE(persister->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(persister->CloseDatabase());
	EXPECT_FALSE(persister->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityPersister::ResetInstance();
}

TEST(EntityPersister, CloseDatabaseWhenDatabaseIsClosed)
{
	EntityPersister *persister = EntityPersister::GetInstance();

	EXPECT_FALSE(persister->GetDatabase().IsOpen());
	EXPECT_NO_THROW(persister->CloseDatabase());

	EntityPersister::ResetInstance();
}

TEST(EntityPersister, PersistFromRoot)
{
	EntityPersisterFixture fixture(HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	EntityPersister *persister = EntityPersister::GetInstance();

	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(persister->Persist(*fixture.GetEntity()));
	EXPECT_NO_THROW(persister->CloseDatabase());

	fixture.VerifyPersistence(fixture.GetRootKey());

	EntityPersister::ResetInstance();
}

TEST(EntityPersister, PersistFromIntermediate)
{
	EntityPersisterFixture fixture(!HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	EntityPersister *persister = EntityPersister::GetInstance();

	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(persister->Persist(*fixture.GetEntity()->GetAggregateMember(fixture.GetIntermKey())));
	EXPECT_NO_THROW(persister->CloseDatabase());

	fixture.VerifyPersistence(fixture.GetIntermKey());

	EntityPersister::ResetInstance();
}

TEST(EntityPersister, PersistFromLeaf)
{
	EntityPersisterFixture fixture(!HAS_ROOT, !HAS_INTERMEDIATE, HAS_LEAF);

	EntityPersister *persister = EntityPersister::GetInstance();

	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	std::shared_ptr<TabEntity> intermEntity = fixture.GetEntity()->GetAggregateMember(fixture.GetIntermKey());
	EXPECT_NO_THROW(
		persister->Persist(*intermEntity->GetAggregateMember(fixture.GetLeafKey())));
	EXPECT_NO_THROW(persister->CloseDatabase());

	fixture.VerifyPersistence(fixture.GetLeafKey());

	EntityPersister::ResetInstance();
}

TEST(EntityPersister, PersistWhenResourcePersisterIsOpen)
{
	EntityPersisterFixture fixture(HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	ResourcePersister *resourcePersister = ResourcePersister::GetInstance();
	resourcePersister->OpenDatabase(DB_PATH);
	EXPECT_TRUE(resourcePersister->GetDatabase().IsOpen());

	EntityPersister *persister = EntityPersister::GetInstance();
	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(persister->Persist(*fixture.GetEntity()));
	EXPECT_NO_THROW(persister->CloseDatabase());

	EXPECT_TRUE(resourcePersister->GetDatabase().IsOpen());

	EntityPersister::ResetInstance();
	ResourcePersister::ResetInstance();
}

TEST(EntityPersister, PersistWhenResourcePersisterIsClosed)
{
	EntityPersisterFixture fixture(HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	ResourcePersister *resourcePersister = ResourcePersister::GetInstance();
	resourcePersister->CloseDatabase();
	EXPECT_FALSE(resourcePersister->GetDatabase().IsOpen());

	EntityPersister *persister = EntityPersister::GetInstance();
	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(persister->Persist(*fixture.GetEntity()));
	EXPECT_NO_THROW(persister->CloseDatabase());

	EXPECT_FALSE(resourcePersister->GetDatabase().IsOpen());

	EntityPersister::ResetInstance();
	ResourcePersister::ResetInstance();
}

TEST(EntityPersister, PersistThrowsWhenGivenEntityWithoutKey)
{
	SqliteRemover dbRemover;

	EntityPersister *persister = EntityPersister::GetInstance();

	EXPECT_NO_THROW(persister->OpenDatabase(DB_PATH));

	TabEntity entity;
	entity.SetKey("");
	EXPECT_THROW(persister->Persist(entity), std::runtime_error);

	EntityPersister::ResetInstance();
}

TEST(EntityPersister, PersistThrowsIfDatabaseIsNotOpen)
{
	SqliteRemover dbRemover;

	EntityPersister *persister = EntityPersister::GetInstance();

	EXPECT_NO_THROW(persister->CloseDatabase());

	TabEntity entity;
	entity.SetKey(ENTITY_1A);
	EXPECT_THROW(persister->Persist(entity), std::runtime_error);

	EntityPersister::ResetInstance();
}
