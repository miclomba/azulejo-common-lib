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

#include "Entities/ISerializableEntity.h"
#include "Entities/EntityAggregationSerializer.h"

namespace fs = std::filesystem;
namespace pt = boost::property_tree;

using entity::Entity;
using entity::EntityAggregationSerializer;
using entity::ISerializableEntity;

using Key = Entity::Key;
using SerializableMemberMap = ISerializableEntity::SerializableMemberMap;

namespace
{
const std::string JSON_ROOT = "C:/users/miclomba/Downloads"; 
const std::string JSON_FILE = "test.json";
const std::string ENTITY_1A = "entity_1a";
const std::string ENTITY_2A = "entity_2a";
const std::string ENTITY_1B = "entity_1b";
const std::string VALUE = "value";

struct TypeA : public ISerializableEntity
{
	TypeA() = default;

	~TypeA()
	{
		if (!path_.empty())
			fs::remove(path_);
	}

	void AggregateProtectedMember(SharedEntity entity) { AggregateMember(std::move(entity)); };

	void Save(pt::ptree& tree, const std::string& path) const override
	{
		path_ = path;
		fs::create_directories(path);
	}

	void Load(pt::ptree& tree, const std::string& path) override
	{
		EXPECT_TRUE(fs::exists(path));
	}

private:
	mutable std::string path_;
};

std::shared_ptr<TypeA> CreateEntity(const Key& root, const std::string& intermediate = "", const Key& leaf = "")
{
	auto rootEntity = std::make_shared<TypeA>();
	auto leafEntity = std::make_shared<TypeA>();
	auto intermediateEntity = std::make_shared<TypeA>();

	rootEntity->SetKey(root);
	intermediateEntity->SetKey(intermediate);
	leafEntity->SetKey(leaf);

	if (!leaf.empty())
		intermediateEntity->AggregateProtectedMember(leafEntity);
	if (!intermediate.empty())
		rootEntity->AggregateProtectedMember(intermediateEntity);

	return rootEntity;
}

std::vector<std::string> LocalSerializationPaths(const std::string& root, const std::string& intermediate = "", const std::string& leaf = "")
{
	std::vector<std::string> directories;

	directories.push_back(root);
	if (!intermediate.empty())
		directories.push_back((fs::path(root) / intermediate).string());
	if (!leaf.empty())
		directories.push_back((fs::path(root) / intermediate / leaf).string());

	return directories;
}

class Fixture
{
public:
	Fixture(bool expectSerialization, const std::string& root, const std::string& intermediate = "", const std::string& leaf = "")
	{
		expectSerialization_ = expectSerialization;

		// get serialization paths
		jsonFile_ = (fs::path(JSON_ROOT) / JSON_FILE).string();
		directoryList_ = LocalSerializationPaths(root, intermediate, leaf);

		// validate serialization
		EXPECT_FALSE(fs::exists(jsonFile_));
		for (std::string& dir : directoryList_)
			EXPECT_FALSE(fs::exists(fs::path(JSON_ROOT) / dir));

		entity_ = CreateEntity(root, intermediate, leaf);
	}

	~Fixture()
	{
		if (expectSerialization_)
		{
			// validate serialization
			EXPECT_TRUE(fs::exists(jsonFile_));
			for (std::string& dir : directoryList_)
				EXPECT_TRUE(fs::exists(fs::path(JSON_ROOT) / dir));

			// cleanup serialization
			fs::remove(jsonFile_);
			EXPECT_FALSE(fs::exists(jsonFile_));
			fs::path directories = fs::path(JSON_ROOT) / entity_->GetKey();
			fs::remove_all(directories);
			EXPECT_FALSE(fs::exists(directories));
		}
	}

	std::string GetJSONFilePath() const
	{
		return jsonFile_;
	}

	std::shared_ptr<TypeA> GetEntity()
	{
		return entity_;
	}

private:
	std::string jsonFile_;
	std::vector<std::string> directoryList_;
	bool expectSerialization_;

	std::shared_ptr<TypeA> entity_;
};
} // end namespace entity

TEST(EntityAggregationSerializer, GetInstance)
{
	EXPECT_NO_THROW(EntityAggregationSerializer::GetInstance());
	EntityAggregationSerializer* serializer = EntityAggregationSerializer::GetInstance();
	EXPECT_TRUE(serializer);
	EXPECT_NO_THROW(EntityAggregationSerializer::ResetInstance());
}

TEST(EntityAggregationSerializer, ResetInstance)
{
	EXPECT_NO_THROW(EntityAggregationSerializer::ResetInstance());
}

TEST(EntityAggregationSerializer, SerializeFromRoot)
{
	Fixture fixture(true, ENTITY_1A, ENTITY_2A, ENTITY_1B);

	EntityAggregationSerializer* serializer = EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(fixture.GetJSONFilePath()));

	EXPECT_NO_THROW(serializer->Serialize(*fixture.GetEntity()));

	EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, SerializeFromIntermediate)
{
	Fixture fixture(true, ENTITY_2A, ENTITY_1B);

	EntityAggregationSerializer* serializer = EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(fixture.GetJSONFilePath()));

	EXPECT_NO_THROW(serializer->Serialize(*fixture.GetEntity()));

	EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, SerializeFromLeaf)
{
	Fixture fixture(true, ENTITY_1B);

	EntityAggregationSerializer* serializer = EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(fixture.GetJSONFilePath()));

	EXPECT_NO_THROW(serializer->Serialize(*fixture.GetEntity()));

	EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, SerializeFromNoKeyEntity)
{
	Fixture fixture(false, ENTITY_1B);

	EntityAggregationSerializer* serializer = EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(fixture.GetJSONFilePath()));

	TypeA entity;
	entity.SetKey("");
	EXPECT_THROW(serializer->Serialize(entity), std::runtime_error);

	EntityAggregationSerializer::ResetInstance();
}
