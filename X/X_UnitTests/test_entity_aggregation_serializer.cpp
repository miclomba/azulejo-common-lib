#include "config.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "X/Entity.h"
#include "X/EntityAggregationSerializer.h"

namespace fs = std::filesystem;
using boost::property_tree::ptree;

namespace
{
const std::string JSON_ROOT = "C:/users/miclomba/Downloads"; 
const std::string JSON_FILE = "test.json";
const std::string ENTITY_1A = "entity_1a";
const std::string ENTITY_2A = "entity_2a";
const std::string ENTITY_1B = "entity_1b";
const std::string VALUE = "value";

class TypeA : public global::Entity
{
public:
	TypeA() = default;

	~TypeA()
	{
		if (!path_.empty())
			std::filesystem::remove(path_);
	}

	void AggregateProtectedMember(std::shared_ptr<Entity> entity) { AggregateMember(std::move(entity)); };
	const Members& GetAggregatedProtectedMembers() { return GetAggregatedMembers(); };
	Entity& GetAggregatedProtectedMember(const std::string& key) { return *GetAggregatedMember(key); }
	const MemberKeys GetAggregatedProtectedMemberKeys() const { return GetAggregatedMemberKeys(); }

	void Save(boost::property_tree::ptree& tree, const std::string& path) const override
	{
		path_ = path;
		std::filesystem::create_directories(path);
	}

	void Load(boost::property_tree::ptree& tree, const std::string& path) override
	{
		EXPECT_TRUE(fs::exists(path));
	}

private:
	mutable std::string path_;
};

std::shared_ptr<TypeA> CreateEntity(const std::string& root, const std::string& intermediate = "", const std::string& leaf = "")
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
		for (auto& dir : directoryList_)
			EXPECT_FALSE(fs::exists(fs::path(JSON_ROOT) / dir));

		entity_ = CreateEntity(root, intermediate, leaf);
	}

	~Fixture()
	{
		if (expectSerialization_)
		{
			// validate serialization
			EXPECT_TRUE(fs::exists(jsonFile_));
			for (auto& dir : directoryList_)
				EXPECT_TRUE(fs::exists(fs::path(JSON_ROOT) / dir));

			// cleanup serialization
			fs::remove(jsonFile_);
			EXPECT_FALSE(fs::exists(jsonFile_));
			auto directories = fs::path(JSON_ROOT) / entity_->GetKey();
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
}

TEST(EntityAggregationSerializer, GetInstance)
{
	EXPECT_NO_THROW(global::EntityAggregationSerializer::GetInstance());
	auto serializer = global::EntityAggregationSerializer::GetInstance();
	EXPECT_TRUE(serializer);
	EXPECT_NO_THROW(global::EntityAggregationSerializer::ResetInstance());
}

TEST(EntityAggregationSerializer, ResetInstance)
{
	EXPECT_NO_THROW(global::EntityAggregationSerializer::ResetInstance());
}

TEST(EntityAggregationSerializer, SerializeFromRoot)
{
	Fixture fixture(true, ENTITY_1A, ENTITY_2A, ENTITY_1B);

	auto serializer = global::EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(fixture.GetJSONFilePath()));

	EXPECT_NO_THROW(serializer->Serialize(*fixture.GetEntity()));

	global::EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, SerializeFromIntermediate)
{
	Fixture fixture(true, ENTITY_2A, ENTITY_1B);

	auto serializer = global::EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(fixture.GetJSONFilePath()));

	EXPECT_NO_THROW(serializer->Serialize(*fixture.GetEntity()));

	global::EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, SerializeFromLeaf)
{
	Fixture fixture(true, ENTITY_1B);

	auto serializer = global::EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(fixture.GetJSONFilePath()));

	EXPECT_NO_THROW(serializer->Serialize(*fixture.GetEntity()));

	global::EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, SerializeFromNoKeyEntity)
{
	Fixture fixture(false, ENTITY_1B);

	auto serializer = global::EntityAggregationSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(fixture.GetJSONFilePath()));

	TypeA entity;
	entity.SetKey("");
	EXPECT_THROW(serializer->Serialize(entity), std::runtime_error);

	global::EntityAggregationSerializer::ResetInstance();
}
