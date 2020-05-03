#include "config.h"

#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "DatabaseAdapters/EntityAggregationDetabularizer.h"
#include "DatabaseAdapters/ITabularizableEntity.h"
#include "DatabaseAdapters/Sqlite.h"
#include "Entities/Entity.h"

namespace fs = std::filesystem;
namespace pt = boost::property_tree;

using database_adapters::EntityAggregationDetabularizer;
using database_adapters::ITabularizableEntity;
using database_adapters::Sqlite;
using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
const std::string JSON_ROOT = (fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY).string(); 
const std::string JSON_FILE = "test.json";
const std::string ENTITY_1A = "entity_1a";
const std::string ENTITY_2A = "entity_2a";
const std::string ENTITY_1B = "entity_1b";
const std::string NON_ENTITY = "not_an_entity";
const std::string BAD_KEY = "bad_key";
const std::string VALUE = "value";
const std::string DB_NAME = "db.sqlite";
const fs::path DB_PATH = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY / DB_NAME;

struct TypeA : public ITabularizableEntity
{
	TypeA() = default;
	~TypeA() = default;

	void AggregateProtectedMember(const Key& key) { AggregateMember(key); };

	std::map<Key, ITabularizableEntity*> GetAggregatedProtectedMembers()
	{ 
		std::map<Key, ITabularizableEntity*> tabularizableMemberMap;

		std::vector<std::string> keys = GetAggregatedMemberKeys<ITabularizableEntity>();
		for (const std::string& key : keys)
		{
			Entity::SharedEntity& member = GetAggregatedMember(key);
			auto memberPtr = static_cast<ITabularizableEntity*>(member.get());
			tabularizableMemberMap.insert(std::make_pair(key,memberPtr));
		}
		return tabularizableMemberMap; 
	};

	SharedEntity GetAggregatedProtectedMember(const Key& key) { return GetAggregatedMember(key); }
	SharedEntity GetAggregatedProtectedMemberPtr(const Key& key) { return GetAggregatedMember(key); }

	void Save(Sqlite& database) const override
	{
	}

	void Load(Sqlite& database) override
	{
		/*
		std::function<int(int, char**, char**)> rowHandler =
			[this](int numCols, char** colValues, char** colNames)
		{
			EXPECT_EQ(COLUMN_NAMES.size(), numCols);
			for (int i = 0; i < numCols; ++i)
			{
				EXPECT_EQ(colNames[i], COLUMN_NAMES[i]);
				EXPECT_EQ(colValues[i], i == 0 ? std::to_string(i) : GetKey());
			}
			return 0;
		};

		std::string sql = "SELECT * FROM " + TABLE_NAME + " WHERE " + COLUMN_NAMES[1] + "='" + GetKey() + "';";
		database.Execute(sql, rowHandler);
		*/
	}
};

void CreateEntityFile(const std::string& filePath)
{
	pt::ptree root;
	root.add_child(ENTITY_1A, pt::ptree());

	pt::ptree& child = root.get_child(ENTITY_1A);
	child.add_child(ENTITY_2A, pt::ptree());

	pt::ptree& child2 = child.get_child(ENTITY_2A);
	child2.add_child(ENTITY_1B, pt::ptree(VALUE));
	// add a child that won't have a corressponding entity
	child2.add_child(NON_ENTITY, pt::ptree(VALUE));

	pt::json_parser::write_json(filePath, root);
}

std::shared_ptr<TypeA> CreateEntityWithUnloadedMember(const Key& root, const Key& leaf)
{
	auto rootEntity = std::make_shared<TypeA>();

	rootEntity->SetKey(root);

	rootEntity->AggregateProtectedMember(leaf);

	return rootEntity;
}

std::string CreateJSONFile()
{
	std::string jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	CreateEntityFile(jsonFile);
	EXPECT_TRUE(fs::exists(jsonFile));
	return jsonFile;
}

void RemoveJSONFile(const std::string& jsonFile)
{
	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));
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

struct EntityAggregationDetabularizerF : public testing::Test
{
	EntityAggregationDetabularizerF()
	{
		// create tabularization structure
		jsonFile_ = (fs::path(JSON_ROOT) / JSON_FILE).string();
		EXPECT_FALSE(fs::exists(jsonFile_));
		CreateEntityFile(jsonFile_);
		EXPECT_TRUE(fs::exists(jsonFile_));
	}

	~EntityAggregationDetabularizerF()
	{
		// cleanup tabularization
		fs::remove(jsonFile_);
		EXPECT_FALSE(fs::exists(jsonFile_));
	}

	std::string GetJSONFile() const
	{
		return jsonFile_;
	}

private:
	SqliteRemover dbRemover_;
	std::string jsonFile_;
};
} // end namespace anonymous

TEST(EntityAggregationDetabularizer, GetInstance)
{
	EXPECT_NO_THROW(EntityAggregationDetabularizer::GetInstance());
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();
	EXPECT_TRUE(detabularizer);
	EXPECT_NO_THROW(EntityAggregationDetabularizer::ResetInstance());
}

TEST(EntityAggregationDetabularizer, ResetInstance)
{
	EXPECT_NO_THROW(EntityAggregationDetabularizer::ResetInstance());
}

TEST(EntityAggregationDetabularizer, ResetInstanceClosesDatabase)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();
	detabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(EntityAggregationDetabularizer::ResetInstance());

	detabularizer = EntityAggregationDetabularizer::GetInstance();
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(EntityAggregationDetabularizer::ResetInstance());
}

TEST(EntityAggregationDetabularizer, LoadSerializationStructure)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	std::string jsonFile = CreateJSONFile(); 

	EXPECT_NO_THROW(detabularizer->LoadSerializationStructure(jsonFile));

	RemoveJSONFile(jsonFile);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, LoadSerializationStructureThrows)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	std::string jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));

	EXPECT_THROW(detabularizer->LoadSerializationStructure(jsonFile), std::runtime_error);
}

TEST(EntityAggregationDetabularizer, HasSerializationStructure)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	std::string jsonFile = CreateJSONFile(); 

	EXPECT_FALSE(detabularizer->HasSerializationStructure());
	EXPECT_NO_THROW(detabularizer->LoadSerializationStructure(jsonFile));
	EXPECT_TRUE(detabularizer->HasSerializationStructure());

	RemoveJSONFile(jsonFile);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, GetSerializationStructure)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	std::string jsonFile = CreateJSONFile();
	EXPECT_NO_THROW(detabularizer->LoadSerializationStructure(jsonFile));

	EXPECT_NO_THROW(detabularizer->GetSerializationStructure());
	pt::ptree structure = detabularizer->GetSerializationStructure();
	EXPECT_TRUE(!structure.empty());

	RemoveJSONFile(jsonFile);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, GetSerializationStructureThrows)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_THROW(detabularizer->GetSerializationStructure(), std::runtime_error);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, HasRegisteredKeyFalse)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->HasRegisteredKey(ENTITY_1A));

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, HasRegisteredKeyTrue)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(detabularizer->HasRegisteredKey(ENTITY_1A));

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, RegisterEntity)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(detabularizer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(detabularizer->HasRegisteredKey(ENTITY_1A));

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, RegisterEntityThrowsUsingEmptyKey)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_THROW(detabularizer->RegisterEntity<TypeA>(""), std::runtime_error);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, RegisterEntityThrowsUsingDuplicateKey)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(detabularizer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(detabularizer->HasRegisteredKey(ENTITY_1A));
	EXPECT_THROW(detabularizer->RegisterEntity<TypeA>(ENTITY_1A), std::runtime_error);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, UnregisterEntity)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(detabularizer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(detabularizer->HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(detabularizer->UnregisterEntity(ENTITY_1A));
	EXPECT_FALSE(detabularizer->HasRegisteredKey(ENTITY_1A));

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, UnregisterEntityThrows)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->HasRegisteredKey(ENTITY_1A));
	EXPECT_THROW(detabularizer->UnregisterEntity(ENTITY_1A), std::runtime_error);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, UnregisterAll)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(detabularizer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(detabularizer->HasRegisteredKey(ENTITY_1A));
	EXPECT_NO_THROW(detabularizer->UnregisterAll());
	EXPECT_FALSE(detabularizer->HasRegisteredKey(ENTITY_1A));

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, GenerateEntity)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	detabularizer->RegisterEntity<TypeA>(ENTITY_1A);
	std::unique_ptr<ITabularizableEntity> entity = detabularizer->GenerateEntity(ENTITY_1A);
	EXPECT_TRUE(entity);
	EXPECT_TRUE(entity->GetKey() == ENTITY_1A);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, GenerateEntityThrows)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->HasRegisteredKey(ENTITY_1A));
	EXPECT_THROW(detabularizer->GenerateEntity(ENTITY_1A), std::runtime_error);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST_F(EntityAggregationDetabularizerF, DetabularizeRoot)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	detabularizer->RegisterEntity<TypeA>(ENTITY_1A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_2A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(detabularizer->LoadSerializationStructure(GetJSONFile()));
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	// detabularize an entity
	TypeA entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(detabularizer->LoadEntity(entity1a));

	// verify
	std::map<Key, ITabularizableEntity*> membersMap = entity1a.GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.size() == size_t(1));
	auto entity2a = std::static_pointer_cast<TypeA>(entity1a.GetAggregatedProtectedMember(ENTITY_2A));
	EXPECT_TRUE(entity2a);
	EXPECT_TRUE(entity2a->GetKey() == ENTITY_2A);

	membersMap = entity2a->GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.size() == size_t(1));
	auto entity1b = std::static_pointer_cast<TypeA>(entity2a->GetAggregatedProtectedMember(ENTITY_1B));
	EXPECT_TRUE(entity1b);
	EXPECT_TRUE(entity1b->GetKey() == ENTITY_1B);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST_F(EntityAggregationDetabularizerF, DetabularizeIntermediate)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	detabularizer->RegisterEntity<TypeA>(ENTITY_1A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_2A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(detabularizer->LoadSerializationStructure(GetJSONFile()));
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	// detabularize an entity
	TypeA entity2a;
	entity2a.SetKey(ENTITY_2A);
	EXPECT_NO_THROW(detabularizer->LoadEntity(entity2a));

	// verify
	std::map<Key, ITabularizableEntity*> membersMap = entity2a.GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.size() == size_t(1));
	auto entity1b = std::static_pointer_cast<TypeA>(entity2a.GetAggregatedProtectedMember(ENTITY_1B));
	EXPECT_TRUE(entity1b);
	EXPECT_TRUE(entity1b->GetKey() == ENTITY_1B);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST_F(EntityAggregationDetabularizerF, DetabularizeLeaf)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	detabularizer->RegisterEntity<TypeA>(ENTITY_1A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_2A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(detabularizer->LoadSerializationStructure(GetJSONFile()));
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	// detabularize an entity
	TypeA entity1b;
	entity1b.SetKey(ENTITY_1B);
	EXPECT_NO_THROW(detabularizer->LoadEntity(entity1b));

	// verify
	std::map<Key, ITabularizableEntity*> membersMap = entity1b.GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.empty());

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, DetabularizeThrowsWhenDatabaseIsNotOpen)
{
	SqliteRemover remover;

	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();
	detabularizer->CloseDatabase();

	TypeA typeA;
	typeA.SetKey(BAD_KEY);

	EXPECT_THROW(detabularizer->LoadEntity(typeA), std::runtime_error);
	EXPECT_EQ(typeA.GetKey(), BAD_KEY);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, DetabularizeReturnsWithoutTabularizationStructure)
{
	SqliteRemover remover;

	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();
	detabularizer->OpenDatabase(DB_PATH);

	TypeA typeA;
	typeA.SetKey(BAD_KEY);

	EXPECT_NO_THROW(detabularizer->LoadEntity(typeA));
	EXPECT_EQ(typeA.GetKey(), BAD_KEY);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST_F(EntityAggregationDetabularizerF, DetabularizeReturnsWithBadKey)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_NO_THROW(detabularizer->LoadSerializationStructure(GetJSONFile()));
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	TypeA badEntity;
	badEntity.SetKey(BAD_KEY);

	EXPECT_NO_THROW(detabularizer->LoadEntity(badEntity));
	EXPECT_EQ(badEntity.GetKey(), BAD_KEY);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST_F(EntityAggregationDetabularizerF, LazyLoadEntity)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	detabularizer->RegisterEntity<TypeA>(ENTITY_1A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_2A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(detabularizer->LoadSerializationStructure(GetJSONFile()));
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	// Create entity with unloaded member
	std::shared_ptr<TypeA> entity = CreateEntityWithUnloadedMember(ENTITY_1A, ENTITY_2A);
	std::map<Key, ITabularizableEntity*> members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(members.find(ENTITY_2A) == members.cend());

	// try to lazy load the entity
	SharedEntity lazyLoaded = entity->GetAggregatedProtectedMember(ENTITY_2A);
	members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(&(*members[ENTITY_2A]) == &(*lazyLoaded));

	EntityAggregationDetabularizer::ResetInstance();
}

TEST_F(EntityAggregationDetabularizerF, LazyLoadEntityWithoutTabularization)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	detabularizer->RegisterEntity<TypeA>(ENTITY_1A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_2A);
	detabularizer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(detabularizer->LoadSerializationStructure(GetJSONFile()));
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));

	// Create entity with unloaded member
	TypeA entity;
	entity.AggregateProtectedMember(BAD_KEY);

	// try to lazy load the entity
	SharedEntity lazyLoaded = entity.GetAggregatedProtectedMemberPtr(BAD_KEY);
	EXPECT_TRUE(lazyLoaded == nullptr);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, GetDatabase)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();
	EXPECT_NO_THROW(detabularizer->GetDatabase());
}

TEST(EntityAggregationDetabularizer, OpenDatabase)
{
	SqliteRemover remover;

	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(detabularizer->OpenDatabase(DB_PATH));
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	detabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(detabularizer->OpenDatabase(DB_PATH), std::runtime_error);

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	detabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(detabularizer->CloseDatabase());
	EXPECT_FALSE(detabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityAggregationDetabularizer::ResetInstance();
}

TEST(EntityAggregationDetabularizer, CloseDatabaseWhenDatabaseIsClosed)
{
	EntityAggregationDetabularizer* detabularizer = EntityAggregationDetabularizer::GetInstance();

	EXPECT_FALSE(detabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(detabularizer->CloseDatabase());

	EntityAggregationDetabularizer::ResetInstance();
}