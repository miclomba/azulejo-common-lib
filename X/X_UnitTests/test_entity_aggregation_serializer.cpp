#include "config.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <gtest/gtest.h>

#include "X/Entity.h"
#include "X/EntityAggregationSerializer.h"

namespace fs = std::filesystem;

namespace
{
const std::string JSON_ROOT = "C:/users/miclomba/Downloads"; 
const std::string JSON_FILE = "test.json";

class TypeA : public global::Entity
{
public:
	TypeA() = default;
	~TypeA() = default;
	void AggregateProtectedMember(std::shared_ptr<Entity> entity) { AggregateMember(std::move(entity)); };
	const Members& GetAggregatedProtectedMembers() { return GetAggregatedMembers(); };
	Entity& GetAggregatedProtectedMember(const std::string& key) { return GetAggregatedMember(key); }
	const MemberKeys GetAggregatedProtectedMemberKeys() const { return GetAggregatedMemberKeys(); }
};

// An entity is always a tree.
// An entity-tree can have many files
const std::string JSON_CONTENT =
R"(
{
  "entity_1a": {
	"entity_2a": {
	},
	"file_2a": "filename"
  },
  "entity_1b": {
    "file_2a": "filename"
  }
}
)";

void CreateFile(const std::string& filePath, const std::string& fileContent)
{
	std::ofstream fileStream(filePath);
	fileStream << fileContent;
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

	CreateFile(jsonFile, JSON_CONTENT);
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
	CreateFile(jsonFile, JSON_CONTENT);
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

	auto entity_1a = std::make_shared<TypeA>(); 
	auto entity_1b = std::make_shared<TypeA>(); 
	auto entity_2a = std::make_shared<TypeA>(); 
	entity_1a->SetKey("entity_1a");
	entity_1b->SetKey("entity_1b");
	entity_2a->SetKey("entity_2a");

	entity_1a->AggregateProtectedMember(entity_2a);

	EXPECT_THROW(serializer->Serialize(*entity_1a), std::runtime_error);

	auto jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));
	CreateFile(jsonFile, JSON_CONTENT);
	EXPECT_TRUE(fs::exists(jsonFile));
	EXPECT_NO_THROW(serializer->LoadSerializationStructure(jsonFile));

	EXPECT_NO_THROW(serializer->Serialize(*entity_1a));

	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));

	global::EntityAggregationSerializer::ResetInstance();
}

TEST(EntityAggregationSerializer, Deserialize)
{
}

TEST(EntityAggregationSerializer, LazyLoadEntity)
{
}