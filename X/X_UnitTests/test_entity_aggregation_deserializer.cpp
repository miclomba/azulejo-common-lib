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

TEST(EntityAggregationDeserializer, Deserialize)
{
	auto deserializer = global::EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(global::Entity()), std::runtime_error);

	// create serialization structure
	auto jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));
	std::vector<std::string> directoryList = CreateEntityFile(jsonFile);
	EXPECT_TRUE(fs::exists(jsonFile));

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(jsonFile));

	// create a serialization
	for (auto& dir : directoryList)
		fs::create_directories(fs::path(JSON_ROOT) / dir);

	// deserialize an entity
	TypeA entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(deserializer->Deserialize(entity1a));
	TypeA entity2a;
	entity2a.SetKey(ENTITY_2A);
	EXPECT_NO_THROW(deserializer->Deserialize(entity2a));
	TypeA entity1b;
	entity1b.SetKey(ENTITY_1B);
	EXPECT_NO_THROW(deserializer->Deserialize(entity1b));

	// cleanup serialization
	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));
	auto directories = fs::path(JSON_ROOT) / entity1a.GetKey();
	fs::remove_all(directories);
	EXPECT_FALSE(fs::exists(directories));

	global::EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, LazyLoadEntity)
{
}