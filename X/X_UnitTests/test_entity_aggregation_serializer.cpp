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

std::shared_ptr<TypeA> CreateEntity()
{
	auto entity_1a = std::make_shared<TypeA>();
	auto entity_1b = std::make_shared<TypeA>();
	auto entity_2a = std::make_shared<TypeA>();
	entity_1a->SetKey(ENTITY_1A);
	entity_1b->SetKey(ENTITY_1B);
	entity_2a->SetKey(ENTITY_2A);

	entity_2a->AggregateProtectedMember(entity_1b);
	entity_1a->AggregateProtectedMember(entity_2a);

	return entity_1a;
}

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
	child.add_child(ENTITY_1A, ptree());
	directories.push_back((fs::path(ENTITY_1A) / ENTITY_1A).string());

	auto& child2 = child.get_child(ENTITY_1A);
	child2.add_child(ENTITY_1A, ptree(VALUE));
	directories.push_back((fs::path(ENTITY_1A) / ENTITY_1A / ENTITY_1A).string());

	boost::property_tree::json_parser::write_json(filePath, root);

	return directories;
}
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

TEST(EntityAggregationSerializer, Serialize)
{
	auto serializer = global::EntityAggregationSerializer::GetInstance();

	// get serialization paths
	auto jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	std::vector<std::string> directoryList = LocalSerializationPaths();

	// validate serialization
	EXPECT_FALSE(fs::exists(jsonFile));
	for (auto& dir : directoryList)
		EXPECT_FALSE(fs::exists(fs::path(JSON_ROOT) / dir));

	// serialize
	EXPECT_NO_THROW(serializer->SetSerializationPath(jsonFile));
	std::shared_ptr<TypeA> entity = CreateEntity();
	EXPECT_NO_THROW(serializer->Serialize(*entity));

	// validate serialization
	EXPECT_TRUE(fs::exists(jsonFile));
	for (auto& dir : directoryList)
		EXPECT_TRUE(fs::exists(fs::path(JSON_ROOT) / dir));

	// cleanup serialization
	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));
	auto directories = fs::path(JSON_ROOT) / entity->GetKey();
	fs::remove_all(directories);
	EXPECT_FALSE(fs::exists(directories));

	global::EntityAggregationSerializer::ResetInstance();
}
