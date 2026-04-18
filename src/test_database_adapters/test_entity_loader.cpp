#include "test_database_adapters/config.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "Config/filesystem.hpp"

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/system/error_code.hpp>

#include "DatabaseAdapters/EntityLoader.h"
#include "DatabaseAdapters/EntityHierarchyBlob.h"
#include "DatabaseAdapters/EntityPersister.h"
#include "DatabaseAdapters/IPersistableEntity.h"
#include "DatabaseAdapters/IPersistableResource.h"
#include "DatabaseAdapters/ResourceLoader.h"
#include "DatabaseAdapters/Sqlite.h"
#include "Entities/Entity.h"
#include "Entities/EntityHierarchy.h"

namespace pt = boost::property_tree;

using boost::system::error_code;
using database_adapters::EntityLoader;
using database_adapters::EntityHierarchyBlob;
using database_adapters::EntityPersister;
using database_adapters::IPersistableEntity;
using database_adapters::IPersistableResource;
using database_adapters::ResourceLoader;
using database_adapters::Sqlite;
using entity::Entity;
using entity::EntityHierarchy;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
	const fs::path ROOT_DIR = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY;
	const std::string DB_NAME = "db.sqlite";
	const fs::path DB_PATH = ROOT_DIR / DB_NAME;
	const std::string ENTITY_1A = "entity_1a";
	const std::string ENTITY_2A = "entity_2a";
	const std::string ENTITY_1B = "entity_1b";
	const std::vector<std::string> ENTITY_KEYS{ENTITY_1A, ENTITY_2A, ENTITY_1B};
	const std::string BAD_KEY = "bad_key";
	const std::string KEY = "key";
	const std::string VAL = "val";
	const std::string LOADED_SUFFIX = "_loaded";
	const std::string VAL_AFTER_LOAD = VAL + LOADED_SUFFIX;
	const std::string ENTITY_HIERARCHY_BLOB_KEY = "entity_hierarchy_blob";

	auto ENTITY_HIERARCHY_BLOB_CONSTRUCTOR = []() -> std::unique_ptr<IPersistableResource>
	{
		return std::make_unique<EntityHierarchyBlob>();
	};

	struct TabEntity : public IPersistableEntity
	{
		TabEntity() = default;
		~TabEntity() = default;

		void AggregateMember(SharedEntity entity)
		{
			IPersistableEntity::AggregateMember(std::move(entity));
		}

		void AggregateMember(const Key &key)
		{
			IPersistableEntity::AggregateMember(key);
		};

		std::shared_ptr<TabEntity> GetAggregateMember(const std::string &key)
		{
			return std::dynamic_pointer_cast<TabEntity>(IPersistableEntity::GetAggregatedMember(key));
		}

		std::map<Key, IPersistableEntity *> GetAggregatedProtectedMembers()
		{
			std::map<Key, IPersistableEntity *> persistableMemberMap;

			for (auto keys = GetAggregatedMemberKeys<IPersistableEntity>(); const std::string &key : keys)
			{
				std::shared_ptr<TabEntity> member = GetAggregateMember(key);
				persistableMemberMap.insert(std::make_pair(key, member.get()));
			}
			return persistableMemberMap;
		};

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

	std::shared_ptr<TabEntity> CreateEntityWithUnloadedMember(const Key &root, const Key &leaf)
	{
		auto rootEntity = std::make_shared<TabEntity>();

		rootEntity->SetKey(root);

		rootEntity->AggregateMember(leaf);

		return rootEntity;
	}

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

	struct EntityLoaderF : public testing::Test
	{
		EntityLoaderF()
		{
			entity_ = CreateEntity(ENTITY_KEYS[0], ENTITY_KEYS[1], ENTITY_KEYS[2]);
			RegisterEntities(ENTITY_1A, ENTITY_2A, ENTITY_1B);

			EntityPersister *persister = EntityPersister::GetInstance();

			persister->OpenDatabase(DB_PATH);
			persister->Persist(*entity_);
			persister->CloseDatabase();

			EntityPersister::ResetInstance();
		}

		~EntityLoaderF()
		{
			ResourceLoader::ResetInstance();
		}

		void VerifyLoading(TabEntity &loadedEntity)
		{
			EXPECT_TRUE(fs::exists(DB_PATH));

			std::string key = loadedEntity.GetKey();

			EXPECT_EQ(loadedEntity.GetValue(), VAL_AFTER_LOAD);
			if (key == ENTITY_1A)
			{
				std::shared_ptr<TabEntity> child = loadedEntity.GetAggregateMember(ENTITY_KEYS[1]);
				EXPECT_EQ(child->GetValue(), VAL_AFTER_LOAD);
				std::shared_ptr<TabEntity> grandChild = child->GetAggregateMember(ENTITY_KEYS[2]);
				EXPECT_EQ(grandChild->GetValue(), VAL_AFTER_LOAD);
			}
			else if (key == ENTITY_2A)
			{
				std::shared_ptr<TabEntity> grandChild = loadedEntity.GetAggregateMember(ENTITY_KEYS[2]);
				EXPECT_EQ(grandChild->GetValue(), VAL_AFTER_LOAD);
			}
		}

	private:
		SqliteRemover dbRemover_;

		std::shared_ptr<TabEntity> entity_;
	};
} // end namespace anonymous

TEST(EntityLoader, GetInstance)
{
	EXPECT_NO_THROW(EntityLoader::GetInstance());
	EntityLoader *loader = EntityLoader::GetInstance();
	EXPECT_TRUE(loader);
	EXPECT_NO_THROW(EntityLoader::ResetInstance());
}

TEST(EntityLoader, ResetInstance)
{
	EXPECT_NO_THROW(EntityLoader::ResetInstance());
}

TEST(EntityLoader, ResetInstanceClosesDatabase)
{
	EntityLoader *loader = EntityLoader::GetInstance();
	loader->OpenDatabase(DB_PATH);
	EXPECT_TRUE(loader->GetDatabase().IsOpen());
	EXPECT_NO_THROW(EntityLoader::ResetInstance());

	loader = EntityLoader::GetInstance();
	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(EntityLoader::ResetInstance());
}

TEST_F(EntityLoaderF, LoaderRoot)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));

	TabEntity entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(loader->LoadEntity(entity1a));

	VerifyLoading(entity1a);

	EntityLoader::ResetInstance();
}

TEST_F(EntityLoaderF, LoadIntermediate)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));

	TabEntity entity2a;
	entity2a.SetKey(ENTITY_2A);
	EXPECT_NO_THROW(loader->LoadEntity(entity2a));

	VerifyLoading(entity2a);

	EntityLoader::ResetInstance();
}

TEST_F(EntityLoaderF, LoadLeaf)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));

	TabEntity entity1b;
	entity1b.SetKey(ENTITY_1B);
	EXPECT_NO_THROW(loader->LoadEntity(entity1b));

	VerifyLoading(entity1b);

	EntityLoader::ResetInstance();
}

TEST_F(EntityLoaderF, LoadDoesntReRegisterHierarchyInDatabase)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	ResourceLoader *resourceLoader = ResourceLoader::GetInstance();
	resourceLoader->RegisterResource<char>(ENTITY_HIERARCHY_BLOB_KEY, ENTITY_HIERARCHY_BLOB_CONSTRUCTOR);

	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));

	ResourceLoader::ResetInstance();
	EntityLoader::ResetInstance();
}

TEST_F(EntityLoaderF, LoadLoadsHierarchyFromDatabase)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));
	EntityHierarchy hierarchy = loader->GetHierarchy();

	EXPECT_TRUE(hierarchy.GetSerializationPath().empty());

	pt::ptree &structure = hierarchy.GetSerializationStructure();
	EXPECT_TRUE(structure.get_child_optional(ENTITY_1A));
	EXPECT_TRUE(structure.get_child_optional(ENTITY_1A + "." + ENTITY_2A));
	EXPECT_TRUE(structure.get_child_optional(ENTITY_1A + "." + ENTITY_2A + "." + ENTITY_1B));

	EntityLoader::ResetInstance();
}

TEST_F(EntityLoaderF, LoadWhenResourceLoaderIsOpen)
{
	ResourceLoader *resourceLoader = ResourceLoader::GetInstance();
	resourceLoader->OpenDatabase(DB_PATH);
	EXPECT_TRUE(resourceLoader->GetDatabase().IsOpen());

	EntityLoader *loader = EntityLoader::GetInstance();
	loader->OpenDatabase(DB_PATH);

	TabEntity entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(loader->LoadEntity(entity1a));

	EXPECT_TRUE(resourceLoader->GetDatabase().IsOpen());

	EntityLoader::ResetInstance();
	ResourceLoader::ResetInstance();
}

TEST_F(EntityLoaderF, LoadWhenResourceLoaderIsClosed)
{
	ResourceLoader *resourceLoader = ResourceLoader::GetInstance();
	resourceLoader->CloseDatabase();
	EXPECT_FALSE(resourceLoader->GetDatabase().IsOpen());

	EntityLoader *loader = EntityLoader::GetInstance();
	loader->OpenDatabase(DB_PATH);

	TabEntity entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(loader->LoadEntity(entity1a));

	EXPECT_FALSE(resourceLoader->GetDatabase().IsOpen());

	EntityLoader::ResetInstance();
	ResourceLoader::ResetInstance();
}

TEST(EntityLoader, LoadWhenResourceLoaderIsOpenButNothingInDatabase)
{
	ResourceLoader *resourceLoader = ResourceLoader::GetInstance();
	resourceLoader->OpenDatabase(DB_PATH);
	EXPECT_TRUE(resourceLoader->GetDatabase().IsOpen());

	EntityLoader *loader = EntityLoader::GetInstance();
	loader->OpenDatabase(DB_PATH);

	EXPECT_EQ(loader->GetHierarchy().GetSerializationStructure(), pt::ptree());

	EXPECT_TRUE(resourceLoader->GetDatabase().IsOpen());

	EntityLoader::ResetInstance();
	ResourceLoader::ResetInstance();
}

TEST(EntityLoader, LoadWhenResourceLoaderIsClosedButNothingInDatabase)
{
	ResourceLoader *resourceLoader = ResourceLoader::GetInstance();
	resourceLoader->CloseDatabase();
	EXPECT_FALSE(resourceLoader->GetDatabase().IsOpen());

	EntityLoader *loader = EntityLoader::GetInstance();
	loader->OpenDatabase(DB_PATH);

	EXPECT_EQ(loader->GetHierarchy().GetSerializationStructure(), pt::ptree());

	EXPECT_FALSE(resourceLoader->GetDatabase().IsOpen());

	EntityLoader::ResetInstance();
	ResourceLoader::ResetInstance();
}

TEST(EntityLoader, LoadThrowsWhenDatabaseIsNotOpen)
{
	SqliteRemover remover;

	EntityLoader *loader = EntityLoader::GetInstance();
	loader->CloseDatabase();

	TabEntity typeA;
	typeA.SetKey(BAD_KEY);

	EXPECT_THROW(loader->LoadEntity(typeA), std::runtime_error);
	EXPECT_EQ(typeA.GetKey(), BAD_KEY);

	EntityLoader::ResetInstance();
}

TEST_F(EntityLoaderF, LoaderReturnsWithBadKey)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));

	TabEntity badEntity;
	badEntity.SetKey(BAD_KEY);

	EXPECT_NO_THROW(loader->LoadEntity(badEntity));
	EXPECT_EQ(badEntity.GetKey(), BAD_KEY);

	EntityLoader::ResetInstance();
}

TEST_F(EntityLoaderF, LazyLoadEntity)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));

	// Create entity with unloaded member
	std::shared_ptr<TabEntity> entity = CreateEntityWithUnloadedMember(ENTITY_1A, ENTITY_2A);
	std::map<Key, IPersistableEntity *> members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(members.find(ENTITY_2A) == members.cend());

	// try to lazy load the entity
	std::shared_ptr<TabEntity> lazyLoaded = entity->GetAggregateMember(ENTITY_2A);
	members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(&(*members[ENTITY_2A]) == &(*lazyLoaded));

	EntityLoader::ResetInstance();
}

TEST_F(EntityLoaderF, LazyLoadEntityWithoutPersistence)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));

	// Create entity with unloaded member
	TabEntity entity;
	entity.AggregateMember(BAD_KEY);

	// try to lazy load the entity
	std::shared_ptr<TabEntity> lazyLoaded = entity.GetAggregateMember(BAD_KEY);
	EXPECT_TRUE(lazyLoaded == nullptr);

	EntityLoader::ResetInstance();
}

TEST(EntityLoader, GetDatabase)
{
	EntityLoader *loader = EntityLoader::GetInstance();
	EXPECT_NO_THROW(loader->GetDatabase());
	EntityLoader::ResetInstance();
}

TEST(EntityLoader, OpenDatabase)
{
	SqliteRemover remover;

	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_FALSE(loader->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(loader->OpenDatabase(DB_PATH));
	EXPECT_TRUE(loader->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityLoader::ResetInstance();
}

TEST(EntityLoader, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	EntityLoader *loader = EntityLoader::GetInstance();

	loader->OpenDatabase(DB_PATH);
	EXPECT_TRUE(loader->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(loader->OpenDatabase(DB_PATH), std::runtime_error);

	EntityLoader::ResetInstance();
}

TEST(EntityLoader, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	EntityLoader *loader = EntityLoader::GetInstance();

	loader->OpenDatabase(DB_PATH);
	EXPECT_TRUE(loader->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(loader->CloseDatabase());
	EXPECT_FALSE(loader->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityLoader::ResetInstance();
}

TEST(EntityLoader, CloseDatabaseWhenDatabaseIsClosed)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_FALSE(loader->GetDatabase().IsOpen());
	EXPECT_NO_THROW(loader->CloseDatabase());

	EntityLoader::ResetInstance();
}

TEST(EntityLoader, GetRegistry)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_NO_THROW(loader->GetRegistry());

	EntityLoader::ResetInstance();
}

TEST(EntityLoader, GetHierarchy)
{
	EntityLoader *loader = EntityLoader::GetInstance();

	EXPECT_NO_THROW(loader->GetHierarchy());

	EntityLoader::ResetInstance();
}