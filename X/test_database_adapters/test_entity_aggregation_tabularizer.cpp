#include "config.h"

#include <array>
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

#include "DatabaseAdapters/ITabularizableEntity.h"
#include "DatabaseAdapters/EntityAggregationTabularizer.h"
#include "DatabaseAdapters/Sqlite.h"
#include "Entities/Entity.h"

namespace fs = std::filesystem;
namespace pt = boost::property_tree;

using database_adapters::EntityAggregationTabularizer;
using database_adapters::ITabularizableEntity;
using database_adapters::Sqlite;
using entity::Entity;

using Key = Entity::Key;

namespace
{
const fs::path ROOT_DIR = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY; 
const std::string JSON_ROOT = ROOT_DIR.string(); 
const std::string JSON_FILE = "test.json";
const std::string ENTITY_1A = "entity_1a";
const std::string ENTITY_2A = "entity_2a";
const std::string ENTITY_1B = "entity_1b";
const std::string DB_NAME = "db.sqlite";
const fs::path DB_PATH = ROOT_DIR / DB_NAME;
const std::string TABLE_NAME = "tbl";
const std::vector<std::string> COLUMN_NAMES{ "pkey","entityKey" };
const std::vector<std::string> ENTITY_KEY_ROW_VALUES{ ENTITY_1A, ENTITY_2A, ENTITY_1B };
const bool HAS_ROOT = true;
const bool HAS_INTERMEDIATE = true;
const bool HAS_LEAF = true;

struct TypeA : public ITabularizableEntity
{
	TypeA() = default;
	~TypeA() = default;

	void AggregateMember(SharedEntity entity) 
	{ 
		entity::Entity::AggregateMember(std::move(entity)); 
	}

	std::shared_ptr<TypeA> GetAggregateMember(const std::string& key) 
	{ 
		return std::dynamic_pointer_cast<TypeA>(entity::Entity::GetAggregatedMember(key)); 
	}

	void Save(Sqlite& database) const override
	{
		std::string sql = "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + " (" + COLUMN_NAMES[0] + " INTEGER PRIMARY KEY AUTOINCREMENT, " + COLUMN_NAMES[1] + " TEXT NOT NULL);";
		database.Execute(sql);

		sql = "INSERT INTO " + TABLE_NAME + " (" + COLUMN_NAMES.at(1) + ") VALUES ('" + GetKey() + "');";
		database.Execute(sql);
	}

	void Load(Sqlite& database) override {}

private:
	std::string value_;
};

std::shared_ptr<TypeA> CreateEntity(const Key& root, const std::string& intermediate, const Key& leaf)
{
	auto rootEntity = std::make_shared<TypeA>();
	auto leafEntity = std::make_shared<TypeA>();
	auto intermediateEntity = std::make_shared<TypeA>();

	rootEntity->SetKey(root);
	intermediateEntity->SetKey(intermediate);
	leafEntity->SetKey(leaf);

	intermediateEntity->AggregateMember(leafEntity);
	rootEntity->AggregateMember(intermediateEntity);

	return rootEntity;
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

struct EntityAggregationTabularizerFixture 
{
	EntityAggregationTabularizerFixture(const bool hasRoot, const bool hasIntermediate, const bool hasLeaf)
	{
		entityMask_ = { hasRoot, hasIntermediate, hasLeaf };

		// get expected output ready
		size_t row = 1;
		for (size_t i = 0; i < entityMask_.size(); ++i)
		{
			if (!entityMask_[i])
				continue;
			pkeyValues_.push_back(std::to_string(row++));
			entityKeyValues_.push_back(ENTITY_KEY_ROW_VALUES[i]);
		}

		// get tabularization paths
		jsonFile_ = (fs::path(JSON_ROOT) / JSON_FILE).string();

		// validate tabularization
		EXPECT_FALSE(fs::exists(jsonFile_));
		EXPECT_FALSE(fs::exists(DB_PATH));

		entity_ = CreateEntity(ENTITY_KEY_ROW_VALUES[0], ENTITY_KEY_ROW_VALUES[1], ENTITY_KEY_ROW_VALUES[2]);
	}

	void VerifyTabularization()
	{
		EXPECT_TRUE(fs::exists(jsonFile_));
		EXPECT_TRUE(fs::exists(DB_PATH));

		size_t row = 0;
		std::function<int(int, char**, char**)> rowHandler =
			[this, &row](int numCols, char** colValues, char** colNames)
		{
			EXPECT_EQ(COLUMN_NAMES.size(), numCols);
			for (int i = 0; i < numCols; ++i)
			{
				std::string colName = colNames[i];
				EXPECT_EQ(colName, COLUMN_NAMES[i]);
				std::string colValue = colValues[i];
				EXPECT_EQ(colValue, colNames[i] == COLUMN_NAMES[0] ? pkeyValues_[row] : entityKeyValues_[row]);
			}
			++row;
			return 0;
		};


		EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();
		for (size_t i = 0; i < entityMask_.size(); ++i)
		{
			if (!entityMask_[i])
				continue;

			std::string sql = "SELECT * FROM " + TABLE_NAME + " WHERE " + COLUMN_NAMES[1] + "='" + ENTITY_KEY_ROW_VALUES[i] + "';";
			tabularizer->GetDatabase().Execute(sql, rowHandler);
		}

		// cleanup tabularization
		fs::remove(jsonFile_);
		EXPECT_FALSE(fs::exists(jsonFile_));
	}

	std::string GetJSONFilePath() const
	{
		return jsonFile_;
	}

	std::shared_ptr<TypeA> GetEntity()
	{
		return entity_;
	}

	std::string GetIntermKey() { return ENTITY_KEY_ROW_VALUES[1]; }
	std::string GetLeafKey() { return ENTITY_KEY_ROW_VALUES[2]; }

private:
	SqliteRemover dbRemover_;

	std::array<bool, 3> entityMask_{false, false, false};
	std::vector<std::string> pkeyValues_;
	std::vector<std::string> entityKeyValues_;

	std::string jsonFile_;

	std::shared_ptr<TypeA> entity_;
};
} // end namespace entity

TEST(EntityAggregationTabularizer, GetInstance)
{
	EXPECT_NO_THROW(EntityAggregationTabularizer::GetInstance());
	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();
	EXPECT_TRUE(tabularizer);
	EXPECT_NO_THROW(EntityAggregationTabularizer::ResetInstance());
}

TEST(EntityAggregationTabularizer, ResetInstance)
{
	EXPECT_NO_THROW(EntityAggregationTabularizer::ResetInstance());
}

TEST(EntityAggregationTabularizer, ResetInstanceClosesDatabase)
{
	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();
	tabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(EntityAggregationTabularizer::ResetInstance());

	tabularizer = EntityAggregationTabularizer::GetInstance();
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_NO_THROW(EntityAggregationTabularizer::ResetInstance());
}

TEST(EntityAggregationTabularizer, SetSerializationPath)
{
	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();
	EXPECT_NO_THROW(tabularizer->SetSerializationPath(JSON_ROOT));
	EXPECT_TRUE(tabularizer->GetSerializationPath() == JSON_ROOT);
}

TEST(EntityAggregationTabularizer, GetSerializationPath)
{
	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();
	tabularizer->SetSerializationPath(JSON_ROOT);
	EXPECT_TRUE(tabularizer->GetSerializationPath() == JSON_ROOT);
}

TEST(EntityAggregationTabularizer, GetDatabase)
{
	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();
	EXPECT_NO_THROW(tabularizer->GetDatabase());
}

TEST(EntityAggregationTabularizer, OpenDatabase)
{
	SqliteRemover remover;

	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();

	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_FALSE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityAggregationTabularizer::ResetInstance();
}

TEST(EntityAggregationTabularizer, OpenDatabaseThrowsIfAlreadyOpen)
{
	SqliteRemover remover;

	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();

	tabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_THROW(tabularizer->OpenDatabase(DB_PATH), std::runtime_error);

	EntityAggregationTabularizer::ResetInstance();
}

TEST(EntityAggregationTabularizer, CloseDatabaseWhenDatabaseIsOpen)
{
	SqliteRemover remover;

	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();

	tabularizer->OpenDatabase(DB_PATH);
	EXPECT_TRUE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));
	EXPECT_NO_THROW(tabularizer->CloseDatabase());
	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_TRUE(fs::exists(DB_PATH));

	EntityAggregationTabularizer::ResetInstance();
}

TEST(EntityAggregationTabularizer, CloseDatabaseWhenDatabaseIsClosed)
{
	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();

	EXPECT_FALSE(tabularizer->GetDatabase().IsOpen());
	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	EntityAggregationTabularizer::ResetInstance();
}

TEST(EntityAggregationTabularizer, TabularizeFromRoot)
{
	EntityAggregationTabularizerFixture fixture(HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->SetSerializationPath(fixture.GetJSONFilePath()));
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));

	EXPECT_NO_THROW(tabularizer->Tabularize(*fixture.GetEntity()));

	fixture.VerifyTabularization();

	EntityAggregationTabularizer::ResetInstance();
}

TEST(EntityAggregationTabularizer, TabularizeFromIntermediate)
{
	EntityAggregationTabularizerFixture fixture(!HAS_ROOT, HAS_INTERMEDIATE, HAS_LEAF);

	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->SetSerializationPath(fixture.GetJSONFilePath()));
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));

	EXPECT_NO_THROW(tabularizer->Tabularize(*fixture.GetEntity()->GetAggregateMember(fixture.GetIntermKey())));

	fixture.VerifyTabularization();

	EntityAggregationTabularizer::ResetInstance();
}

TEST(EntityAggregationTabularizer, TabularizeFromLeaf)
{
	EntityAggregationTabularizerFixture fixture(!HAS_ROOT, !HAS_INTERMEDIATE, HAS_LEAF);

	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->SetSerializationPath(fixture.GetJSONFilePath()));
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));

	std::shared_ptr<TypeA> intermEntity = fixture.GetEntity()->GetAggregateMember(fixture.GetIntermKey());
	EXPECT_NO_THROW(
		tabularizer->Tabularize(*intermEntity->GetAggregateMember(fixture.GetLeafKey()))
	);

	fixture.VerifyTabularization();

	EntityAggregationTabularizer::ResetInstance();
}

TEST(EntityAggregationTabularizer, TabularizeThrowsWhenGivenEntityWithoutKey)
{
	EntityAggregationTabularizerFixture fixture(HAS_ROOT, !HAS_INTERMEDIATE, !HAS_LEAF);

	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->SetSerializationPath(fixture.GetJSONFilePath()));
	EXPECT_NO_THROW(tabularizer->OpenDatabase(DB_PATH));

	TypeA entity;
	entity.SetKey("");
	EXPECT_THROW(tabularizer->Tabularize(entity), std::runtime_error);

	EntityAggregationTabularizer::ResetInstance();
}

TEST(EntityAggregationTabularizer, TabularizeThrowsIfDatabaseIsNotOpen)
{
	EntityAggregationTabularizerFixture fixture(HAS_ROOT, !HAS_INTERMEDIATE, !HAS_LEAF);

	EntityAggregationTabularizer* tabularizer = EntityAggregationTabularizer::GetInstance();

	EXPECT_NO_THROW(tabularizer->SetSerializationPath(fixture.GetJSONFilePath()));
	EXPECT_NO_THROW(tabularizer->CloseDatabase());

	TypeA entity;
	entity.SetKey(ENTITY_1A);
	EXPECT_THROW(tabularizer->Tabularize(entity), std::runtime_error);

	EntityAggregationTabularizer::ResetInstance();
}

