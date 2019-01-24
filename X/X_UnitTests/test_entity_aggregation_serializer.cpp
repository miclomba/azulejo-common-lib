#include "config.h"

#include <filesystem>
#include <fstream>
#include <string>
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

void CreateEntityFile(const std::string& filePath)
{
	ptree root;
	root.add_child(ENTITY_1A, ptree());
	auto& child = root.get_child(ENTITY_1A);
	child.add_child(ENTITY_2A, ptree());
	auto& child2 = child.get_child(ENTITY_2A);
	child2.add_child(ENTITY_1B, ptree(VALUE));

	boost::property_tree::json_parser::write_json(filePath, root);
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

TEST(EntityAggregationSerializer, LoadSerializationStructure)
{
	auto serializer = global::EntityAggregationSerializer::GetInstance();

	auto jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));

	EXPECT_THROW(serializer->LoadSerializationStructure(jsonFile), std::runtime_error);

	CreateEntityFile(jsonFile);
	EXPECT_TRUE(fs::exists(jsonFile));

	EXPECT_NO_THROW(serializer->LoadSerializationStructure(jsonFile));

	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));

	global::EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, HasSerializationStructure)
{
	auto serializer = global::EntityAggregationSerializer::GetInstance();

	auto jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));
	CreateEntityFile(jsonFile);
	EXPECT_TRUE(fs::exists(jsonFile));

	EXPECT_FALSE(serializer->HasSerializationStructure());
	EXPECT_NO_THROW(serializer->LoadSerializationStructure(jsonFile));
	EXPECT_TRUE(serializer->HasSerializationStructure());

	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));

	global::EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, Serialize)
{
	auto serializer = global::EntityAggregationSerializer::GetInstance();

	auto entity_1a = CreateEntity();

	EXPECT_THROW(serializer->Serialize(*entity_1a), std::runtime_error);

	auto jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));
	CreateEntityFile(jsonFile);
	EXPECT_TRUE(fs::exists(jsonFile));

	EXPECT_NO_THROW(serializer->LoadSerializationStructure(jsonFile));

	EXPECT_NO_THROW(serializer->Serialize(*entity_1a));

	// validate all directories were created

	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));

	auto directories = fs::path(JSON_ROOT) / entity_1a->GetKey();
	fs::remove_all(directories);
	EXPECT_FALSE(fs::exists(directories));

	global::EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, Deserialize)
{
}

TEST(EntityAggregationSerializer, LazyLoadEntity)
{
}