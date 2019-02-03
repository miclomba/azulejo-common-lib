#include "config.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "X/Entity.h"
#include "X/EntityAggregationDeserializer.h"

namespace fs = std::filesystem;
using boost::property_tree::ptree;

namespace
{
const std::string JSON_ROOT = "C:/users/miclomba/Downloads"; 
const std::string JSON_FILE = "test.json";
const std::string ENTITY_1A = "entity_1a";
const std::string ENTITY_2A = "entity_2a";
const std::string ENTITY_1B = "entity_1b";
const std::string BAD_KEY = "bad_key";
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
	Entity& GetAggregatedProtectedMember(const std::string& key) { return GetAggregatedMember(key); }
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

std::vector<std::string> LocalSerializationPaths()
{
	std::vector<std::string> directories;

	directories.push_back(ENTITY_1A);
	directories.push_back((fs::path(ENTITY_1A) / ENTITY_2A).string());
	directories.push_back((fs::path(ENTITY_1A) / ENTITY_2A / ENTITY_1B).string());

	return directories;
}

std::vector<std::string> CreateEntityFile(const std::string& filePath)
{
	std::vector<std::string> directories;

	ptree root;
	root.add_child(ENTITY_1A, ptree());
	directories.push_back(ENTITY_1A);

	auto& child = root.get_child(ENTITY_1A);
	child.add_child(ENTITY_2A, ptree());
	directories.push_back((fs::path(ENTITY_1A) / ENTITY_2A).string());

	auto& child2 = child.get_child(ENTITY_2A);
	child2.add_child(ENTITY_1B, ptree(VALUE));
	directories.push_back((fs::path(ENTITY_1A) / ENTITY_2A / ENTITY_1B).string());

	boost::property_tree::json_parser::write_json(filePath, root);

	return directories;
}

class EntityAggregationDeserialization : public testing::Test
{
public:
	EntityAggregationDeserialization()
	{
		// create serialization structure
		jsonFile_ = (fs::path(JSON_ROOT) / JSON_FILE).string();
		EXPECT_FALSE(fs::exists(jsonFile_));
		directoryList_ = CreateEntityFile(jsonFile_);
		EXPECT_TRUE(fs::exists(jsonFile_));

		// create a serialization
		for (auto& dir : directoryList_)
		{
			fs::path d = fs::path(JSON_ROOT) / dir;
			EXPECT_FALSE(fs::exists(d));
			fs::create_directories(d);
			EXPECT_TRUE(fs::exists(d));
		}
	}

	~EntityAggregationDeserialization()
	{
		// cleanup serialization
		fs::remove(jsonFile_);
		EXPECT_FALSE(fs::exists(jsonFile_));
		auto directories = fs::path(JSON_ROOT) / directoryList_[0];
		fs::remove_all(directories);
		EXPECT_FALSE(fs::exists(directories));
	}

	std::string GetJSONFile() const
	{
		return jsonFile_;
	}

private:
	std::string jsonFile_;
	std::vector<std::string> directoryList_;
};
}

TEST(EntityAggregationDeserializer, GetInstance)
{
	EXPECT_NO_THROW(global::EntityAggregationDeserializer::GetInstance());
	auto deserializer = global::EntityAggregationDeserializer::GetInstance();
	EXPECT_TRUE(deserializer);
	EXPECT_NO_THROW(global::EntityAggregationDeserializer::ResetInstance());
}

TEST(EntityAggregationDeserializer, ResetInstance)
{
	EXPECT_NO_THROW(global::EntityAggregationDeserializer::ResetInstance());
}

TEST(EntityAggregationDeserializer, LoadSerializationStructure)
{
	auto deserializer = global::EntityAggregationDeserializer::GetInstance();

	auto jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));

	EXPECT_THROW(deserializer->LoadSerializationStructure(jsonFile), std::runtime_error);

	CreateEntityFile(jsonFile);
	EXPECT_TRUE(fs::exists(jsonFile));

	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(jsonFile));

	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));

	global::EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, HasSerializationStructure)
{
	auto deserializer = global::EntityAggregationDeserializer::GetInstance();

	auto jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));
	CreateEntityFile(jsonFile);
	EXPECT_TRUE(fs::exists(jsonFile));

	EXPECT_FALSE(deserializer->HasSerializationStructure());
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(jsonFile));
	EXPECT_TRUE(deserializer->HasSerializationStructure());

	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));

	global::EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, DeserializeRoot)
{
	auto deserializer = global::EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(global::Entity()), std::runtime_error);

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(deserializer->Deserialize(entity1a));

	global::EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, DeserializeIntermediate)
{
	auto deserializer = global::EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(global::Entity()), std::runtime_error);

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity2a;
	entity2a.SetKey(ENTITY_2A);
	EXPECT_NO_THROW(deserializer->Deserialize(entity2a));

	global::EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, DeserializeLeaf)
{
	auto deserializer = global::EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(global::Entity()), std::runtime_error);

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity1b;
	entity1b.SetKey(ENTITY_1B);
	EXPECT_NO_THROW(deserializer->Deserialize(entity1b));

	global::EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, DeserializeBadKey)
{
	auto deserializer = global::EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(global::Entity()), std::runtime_error);

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA badEntity;
	badEntity.SetKey(BAD_KEY);
	EXPECT_THROW(deserializer->Deserialize(badEntity), std::runtime_error);

	global::EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, LazyLoadEntity)
{
}