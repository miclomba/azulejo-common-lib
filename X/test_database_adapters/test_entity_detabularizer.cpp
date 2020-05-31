#include "config.h"

#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

#include "DatabaseAdapters/EntityDetabularizer.h"
#include "DatabaseAdapters/EntityHierarchyBlob.h"
#include "DatabaseAdapters/EntityTabularizer.h"
#include "DatabaseAdapters/ITabularizableEntity.h"
#include "DatabaseAdapters/ITabularizableResource.h"
#include "DatabaseAdapters/ResourceDetabularizer.h"
#include "DatabaseAdapters/Sqlite.h"
#include "Entities/Entity.h"
#include "Entities/EntityHierarchy.h"

namespace fs = std::filesystem;
namespace pt = boost::property_tree;

using database_adapters::EntityDetabularizer;
using database_adapters::EntityHierarchyBlob;
using database_adapters::EntityTabularizer;
using database_adapters::ITabularizableEntity;
using database_adapters::ITabularizableResource;
using database_adapters::ResourceDetabularizer;
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
const std::vector<std::string> ENTITY_KEYS{ ENTITY_1A, ENTITY_2A, ENTITY_1B };
const std::string BAD_KEY = "bad_key";
const std::string KEY = "key";
const std::string VAL = "val";
const std::string LOADED_SUFFIX = "_loaded";
const std::string VAL_AFTER_LOAD = VAL + LOADED_SUFFIX;
const std::string ENTITY_HIERARCHY_BLOB_KEY = "entity_hierarchy_blob";

auto ENTITY_HIERARCHY_BLOB_CONSTRUCTOR = []()->std::unique_ptr<ITabularizableResource>
{
	return std::make_unique<EntityHierarchyBlob>();
};

struct TabEntity : public ITabularizableEntity
{
	TabEntity() = default;
	~TabEntity() = default;

	void AggregateMember(SharedEntity entity)
	{
		ITabularizableEntity::AggregateMember(std::move(entity));
	}

	void AggregateMember(const Key& key) 
	{ 
		ITabularizableEntity::AggregateMember(key); 
	};

	std::shared_ptr<TabEntity> GetAggregateMember(const std::string& key)
	{
		return std::dynamic_pointer_cast<TabEntity>(ITabularizableEntity::GetAggregatedMember(key));
	}

	std::map<Key, ITabularizableEntity*> GetAggregatedProtectedMembers()
	{ 
		std::map<Key, ITabularizableEntity*> tabularizableMemberMap;

		std::vector<std::string> keys = GetAggregatedMemberKeys<ITabularizableEntity>();
		for (const std::string& key : keys)
		{
			std::shared_ptr<TabEntity> member = GetAggregateMember(key);
			tabularizableMemberMap.insert(std::make_pair(key,member.get()));
		}
		return tabularizableMemberMap; 
	};

	void Save(pt::ptree& tree, Sqlite& database) const override
	{
		tree.put(KEY, value_);
	}

	void Load(pt::ptree& tree, Sqlite& database) override
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

std::shared_ptr<TabEntity> CreateEntityWithUnloadedMember(const Key& root, const Key& leaf)
{
	auto rootEntity = std::make_shared<TabEntity>();

	rootEntity->SetKey(root);

	rootEntity->AggregateMember(leaf);

	return rootEntity;
}

std::shared_ptr<TabEntity> CreateEntity(const Key& root, const std::string& intermediate, const Key& leaf)
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

void RegisterEntities(const Key& root, const std::string& intermediate, const Key& leaf)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();
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
			fs::permissions(DB_PATH, fs::perms::all, fs::perm_options::add);
			fs::remove(DB_PATH);
		}
	}
};

struct EntityDetabularizerF : public testing::Test
{
	EntityDetabularizerF()
	{
		entity_ = CreateEntity(ENTITY_KEYS[0], ENTITY_KEYS[1], ENTITY_KEYS[2]);
		RegisterEntities(ENTITY_1A, ENTITY_2A, ENTITY_1B);

		EntityTabularizer* tabularizer = EntityTabularizer::GetInstance();

		tabularizer->OpenDatabase(DB_PATH);
		tabularizer->Tabularize(*entity_);
		tabularizer->CloseDatabase();

		EntityTabularizer::ResetInstance();
	}

	~EntityDetabularizerF()
	{
		ResourceDetabularizer::ResetInstance();
	}

	void VerifyDetabularization(TabEntity& loadedEntity)
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

TEST(EntityDetabularizer, GetInstance)
{
	EXPECT_NO_THROW(EntityDetabularizer::GetInstance());
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();
	EXPECT_TRUE(detabularizer);
	EXPECT_NO_THROW(EntityDetabularizer::ResetInstance());
}

TEST(EntityDetabularizer, ResetInstance)
{
	EXPECT_NO_THROW(EntityDetabularizer::ResetInstance());
}

TEST(EntityDetabularizer, ResetInstanceClosesDatabase)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();
	detabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(EntityDetabularizer::ResetInstance());

	detabularizer = EntityDetabularizer::GetInstance();
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(EntityDetabularizer::ResetInstance());
}

TEST_F(EntityDetabularizerF, DetabularizeRoot)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	TabEntity entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(detabularizer->LoadEntity(entity1a));

	VerifyDetabularization(entity1a);

	EntityDetabularizer::ResetInstance();
}

TEST_F(EntityDetabularizerF, DetabularizeIntermediate)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	TabEntity entity2a;
	entity2a.SetKey(ENTITY_2A);
	EXPECT_NO_THROW(detabularizer->LoadEntity(entity2a));

	VerifyDetabularization(entity2a);

	EntityDetabularizer::ResetInstance();
}

TEST_F(EntityDetabularizerF, DetabularizeLeaf)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	TabEntity entity1b;
	entity1b.SetKey(ENTITY_1B);
	EXPECT_NO_THROW(detabularizer->LoadEntity(entity1b));

	VerifyDetabularization(entity1b);

	EntityDetabularizer::ResetInstance();
}

TEST_F(EntityDetabularizerF, DetabularizeDoesntReRegisterHierarchyInDatabase)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	ResourceDetabularizer* resourceDetabularizer = ResourceDetabularizer::GetInstance();
	resourceDetabularizer->RegisterResource<char>(ENTITY_HIERARCHY_BLOB_KEY, ENTITY_HIERARCHY_BLOB_CONSTRUCTOR);

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	ResourceDetabularizer::ResetInstance();
	EntityDetabularizer::ResetInstance();
}

TEST_F(EntityDetabularizerF, DetabularizeLoadsHierarchyFromDatabase)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));
	EntityHierarchy hierarchy = detabularizer->GetHierarchy();

	EXPECT_TRUE(hierarchy.GetSerializationPath().empty());
	
	pt::ptree& structure = hierarchy.GetSerializationStructure();
	EXPECT_TRUE(structure.get_child_optional(ENTITY_1A));
	EXPECT_TRUE(structure.get_child_optional(ENTITY_1A + "." + ENTITY_2A));
	EXPECT_TRUE(structure.get_child_optional(ENTITY_1A + "." + ENTITY_2A + "." + ENTITY_1B));

	EntityDetabularizer::ResetInstance();
}

TEST_F(EntityDetabularizerF, DetabularizeWhenResourceDetabularizerIsOpen)
{
	ResourceDetabularizer* resourceDetabularizer = ResourceDetabularizer::GetInstance();
	resourceDetabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(resourceDetabularizer->GetDatabase().IsOpen());

	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();
	detabularizer->OpenDatabase(DB_PATH);

	TabEntity entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(detabularizer->LoadEntity(entity1a));

	EXPECT_TRUE(resourceDetabularizer->GetDatabase().IsOpen());

	EntityDetabularizer::ResetInstance();
	ResourceDetabularizer::ResetInstance();
}

TEST_F(EntityDetabularizerF, DetabularizeWhenResourceDetabularizerIsClosed)
{
	ResourceDetabularizer* resourceDetabularizer = ResourceDetabularizer::GetInstance();
	resourceDetabularizer->CloseDatabase();
	EXPECT_FALSE(resourceDetabularizer->GetDatabase().IsOpen());

	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();
	detabularizer->OpenDatabase(DB_PATH);

	TabEntity entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(detabularizer->LoadEntity(entity1a));

	EXPECT_FALSE(resourceDetabularizer->GetDatabase().IsOpen());

	EntityDetabularizer::ResetInstance();
	ResourceDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, DetabularizeWhenResourceDetabularizerIsOpenButNothingInDatabase)
{
	ResourceDetabularizer* resourceDetabularizer = ResourceDetabularizer::GetInstance();
	resourceDetabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(resourceDetabularizer->GetDatabase().IsOpen());

	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();
	detabularizer->OpenDatabase(DB_PATH);

	EXPECT_EQ(detabularizer->GetHierarchy().GetSerializationStructure(), pt::ptree());

	EXPECT_TRUE(resourceDetabularizer->GetDatabase().IsOpen());

	EntityDetabularizer::ResetInstance();
	ResourceDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, DetabularizeWhenResourceDetabularizerIsClosedButNothingInDatabase)
{
	ResourceDetabularizer* resourceDetabularizer = ResourceDetabularizer::GetInstance();
	resourceDetabularizer->CloseDatabase();
	EXPECT_FALSE(resourceDetabularizer->GetDatabase().IsOpen());

	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();
	detabularizer->OpenDatabase(DB_PATH);

	EXPECT_EQ(detabularizer->GetHierarchy().GetSerializationStructure(), pt::ptree());

	EXPECT_FALSE(resourceDetabularizer->GetDatabase().IsOpen());

	EntityDetabularizer::ResetInstance();
	ResourceDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, DetabularizeThrowsWhenDatabaseIsNotOpen)
{
	SqliteRemover remover;

	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();
	detabularizer->CloseDatabase();

	TabEntity typeA;
	typeA.SetKey(BAD_KEY);

	EXPECT_THROW(detabularizer->LoadEntity(typeA), std::runtime_error);
	EXPECT_EQ(typeA.GetKey(), BAD_KEY);

	EntityDetabularizer::ResetInstance();
}

TEST_F(EntityDetabularizerF, DetabularizeReturnsWithBadKey)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	TabEntity badEntity;
	badEntity.SetKey(BAD_KEY);

	EXPECT_NO_THROW(detabularizer->LoadEntity(badEntity));
	EXPECT_EQ(badEntity.GetKey(), BAD_KEY);

	EntityDetabularizer::ResetInstance();
}

TEST_F(EntityDetabularizerF, LazyLoadEntity)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	// Create entity with unloaded member
	std::shared_ptr<TabEntity> entity = CreateEntityWithUnloadedMember(ENTITY_1A, ENTITY_2A);
	std::map<Key, ITabularizableEntity*> members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(members.find(ENTITY_2A) == members.cend());

	// try to lazy load the entity
	std::shared_ptr<TabEntity> lazyLoaded = entity->GetAggregateMember(ENTITY_2A);
	members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(&(*members[ENTITY_2A]) == &(*lazyLoaded));

	EntityDetabularizer::ResetInstance();
}

TEST_F(EntityDetabularizerF, LazyLoadEntityWithoutTabularization)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	// Create entity with unloaded member
	TabEntity entity;
	entity.AggregateMember(BAD_KEY);

	// try to lazy load the entity
	std::shared_ptr<TabEntity> lazyLoaded = entity.GetAggregateMember(BAD_KEY);
	EXPECT_TRUE(lazyLoaded == nullptr);

	EntityDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, GetDatabase)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();
	EXPECT_NO_THROW(detabularizer->GetDatabase());
	EntityDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, OpenDatabase)
{
	SqliteRemover remover;

	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	detabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(detabularizer->OpenDatabase(DB_PATH), std::runtime_error);

	EntityDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	detabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(detabularizer->CloseDatabase());
	EXPECT_FALSE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, CloseDatabaseWhenDatabaseIsClosed)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(detabularizer->CloseDatabase());

	EntityDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, GetRegistry)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->GetRegistry());

	EntityDetabularizer::ResetInstance();
}

TEST(EntityDetabularizer, GetHierarchy)
{
	EntityDetabularizer* detabularizer = EntityDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->GetHierarchy());

	EntityDetabularizer::ResetInstance();
}