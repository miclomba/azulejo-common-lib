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

#include "FilesystemAdapters/ISerializableEntity.h"
#include "FilesystemAdapters/EntityAggregationSerializer.h"

namespace fs = std::filesystem;
namespace pt = boost::property_tree;

using entity::Entity;
using filesystem_adapters::EntityAggregationSerializer;
using filesystem_adapters::ISerializableEntity;

using Key = Entity::Key;

namespace
{
const std::string JSON_ROOT = (fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY).string(); 
const std::string JSON_FILE = "test.json";
const std::string ENTITY_1A = "entity_1a";
const std::string ENTITY_2A = "entity_2a";
const std::string ENTITY_1B = "entity_1b";

struct TypeA : public ISerializableEntity
{
	TypeA() = default;

	~TypeA()
	{
		if (!path_.empty())
			fs::remove(path_);
	}

	void AggregateMember(SharedEntity entity) 
	{ 
		entity::Entity::AggregateMember(std::move(entity)); 
	}

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
		intermediateEntity->AggregateMember(leafEntity);
	if (!intermediate.empty())
		rootEntity->AggregateMember(intermediateEntity);

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

struct EntityAggregationSerializerFixture 
{
	EntityAggregationSerializerFixture(const std::string& root, const std::string& intermediate = "", const std::string& leaf = "")
	{
		// get serialization paths
		jsonFile_ = (fs::path(JSON_ROOT) / JSON_FILE).string();
		directoryList_ = LocalSerializationPaths(root, intermediate, leaf);

		// validate serialization
		EXPECT_FALSE(fs::exists(jsonFile_));
		for (std::string& dir : directoryList_)
			EXPECT_FALSE(fs::exists(fs::path(JSON_ROOT) / dir));

		entity_ = CreateEntity(root, intermediate, leaf);
	}

	void VerifySerialization()
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
	EntityAggregationSerializerFixture fixture(ENTITY_1A, ENTITY_2A, ENTITY_1B);

	EntityAggregationSerializer* serializer = EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->GetHierarchy().SetSerializationPath(fixture.GetJSONFilePath()));

	EXPECT_NO_THROW(serializer->Serialize(*fixture.GetEntity()));

	fixture.VerifySerialization();

	EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, SerializeFromIntermediate)
{
	EntityAggregationSerializerFixture fixture(ENTITY_2A, ENTITY_1B);

	EntityAggregationSerializer* serializer = EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->GetHierarchy().SetSerializationPath(fixture.GetJSONFilePath()));

	EXPECT_NO_THROW(serializer->Serialize(*fixture.GetEntity()));

	fixture.VerifySerialization();

	EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, SerializeFromLeaf)
{
	EntityAggregationSerializerFixture fixture(ENTITY_1B);

	EntityAggregationSerializer* serializer = EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->GetHierarchy().SetSerializationPath(fixture.GetJSONFilePath()));

	EXPECT_NO_THROW(serializer->Serialize(*fixture.GetEntity()));

	fixture.VerifySerialization();

	EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, SerializeThrows)
{
	EntityAggregationSerializerFixture fixture(ENTITY_1B);

	EntityAggregationSerializer* serializer = EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->GetHierarchy().SetSerializationPath(fixture.GetJSONFilePath()));

	TypeA entity;
	entity.SetKey("");
	EXPECT_THROW(serializer->Serialize(entity), std::runtime_error);

	EntityAggregationSerializer::ResetInstance();
}
