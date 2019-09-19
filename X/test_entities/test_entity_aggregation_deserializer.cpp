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
#include "Entities/EntityAggregationDeserializer.h"

namespace fs = std::filesystem;
namespace pt = boost::property_tree;

using entity::Entity;
using entity::EntityAggregationDeserializer;
using entity::ISerializableEntity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;
using SharedSerializable = ISerializableEntity::SharedSerializable;
using SerializationMembersMap = ISerializableEntity::SerializableMemberMap;

#ifdef _WIN32
#define ROOT "C:/"
#else
#define ROOT "/"
#endif

namespace
{
const std::string JSON_ROOT = (fs::path(ROOT) / "users" / "miclomba" / "Downloads").string(); 
const std::string JSON_FILE = "test.json";
const std::string ENTITY_1A = "entity_1a";
const std::string ENTITY_2A = "entity_2a";
const std::string ENTITY_1B = "entity_1b";
const std::string BAD_KEY = "bad_key";
const std::string VALUE = "value";

struct TypeA : public ISerializableEntity
{
	TypeA() = default;
	~TypeA()
	{
		if (!path_.empty())
			fs::remove(path_);
	}

	void AggregateProtectedMember(const Key& key) { AggregateMember(key); };

	const SerializationMembersMap GetAggregatedProtectedMembers() { return GetAggregatedMembers(); };
	SharedEntity GetAggregatedProtectedMember(const Key& key) { return GetAggregatedMember(key); }
	SharedEntity GetAggregatedProtectedMemberPtr(const Key& key) { return GetAggregatedMember(key); }

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

std::vector<std::string> CreateEntityFile(const std::string& filePath)
{
	std::vector<std::string> directories;

	pt::ptree root;
	root.add_child(ENTITY_1A, pt::ptree());
	directories.push_back(ENTITY_1A);

	pt::ptree& child = root.get_child(ENTITY_1A);
	child.add_child(ENTITY_2A, pt::ptree());
	directories.push_back((fs::path(ENTITY_1A) / ENTITY_2A).string());

	pt::ptree& child2 = child.get_child(ENTITY_2A);
	child2.add_child(ENTITY_1B, pt::ptree(VALUE));
	directories.push_back((fs::path(ENTITY_1A) / ENTITY_2A / ENTITY_1B).string());

	pt::json_parser::write_json(filePath, root);

	return directories;
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

struct EntityAggregationDeserializerFixture : public testing::Test
{
	EntityAggregationDeserializerFixture()
	{
		// create serialization structure
		jsonFile_ = (fs::path(JSON_ROOT) / JSON_FILE).string();
		EXPECT_FALSE(fs::exists(jsonFile_));
		directoryList_ = CreateEntityFile(jsonFile_);
		EXPECT_TRUE(fs::exists(jsonFile_));

		// create a serialization
		for (std::string& dir : directoryList_)
		{
			fs::path d = fs::path(JSON_ROOT) / dir;
			EXPECT_FALSE(fs::exists(d));
			fs::create_directories(d);
			EXPECT_TRUE(fs::exists(d));
		}
	}

	~EntityAggregationDeserializerFixture()
	{
		// cleanup serialization
		fs::remove(jsonFile_);
		EXPECT_FALSE(fs::exists(jsonFile_));
		fs::path directories = fs::path(JSON_ROOT) / directoryList_[0];
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
} // end namespace anonymous

TEST(EntityAggregationDeserializer, GetInstance)
{
	EXPECT_NO_THROW(EntityAggregationDeserializer::GetInstance());
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();
	EXPECT_TRUE(deserializer);
	EXPECT_NO_THROW(EntityAggregationDeserializer::ResetInstance());
}

TEST(EntityAggregationDeserializer, ResetInstance)
{
	EXPECT_NO_THROW(EntityAggregationDeserializer::ResetInstance());
}

TEST(EntityAggregationDeserializer, LoadSerializationStructure)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	std::string jsonFile = CreateJSONFile(); 

	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(jsonFile));

	RemoveJSONFile(jsonFile);

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, LoadSerializationStructureThrows)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	std::string jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));

	EXPECT_THROW(deserializer->LoadSerializationStructure(jsonFile), std::runtime_error);
}

TEST(EntityAggregationDeserializer, HasSerializationStructure)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	std::string jsonFile = CreateJSONFile(); 

	EXPECT_FALSE(deserializer->HasSerializationStructure());
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(jsonFile));
	EXPECT_TRUE(deserializer->HasSerializationStructure());

	RemoveJSONFile(jsonFile);

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, HasSerializationKeyFalse)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(ENTITY_1A));

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, HasSerializationKeyTrue)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(deserializer->HasSerializationKey(ENTITY_1A));

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, RegisterEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(ENTITY_1A));
	EXPECT_NO_THROW(deserializer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(deserializer->HasSerializationKey(ENTITY_1A));

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, RegisterEntityThrowsUsingEmptyKey)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_THROW(deserializer->RegisterEntity<TypeA>(""), std::runtime_error);

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, RegisterEntityThrowsUsingDuplicateKey)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(ENTITY_1A));
	EXPECT_NO_THROW(deserializer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(deserializer->HasSerializationKey(ENTITY_1A));
	EXPECT_THROW(deserializer->RegisterEntity<TypeA>(ENTITY_1A), std::runtime_error);

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, UnregisterEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(ENTITY_1A));
	EXPECT_NO_THROW(deserializer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_TRUE(deserializer->HasSerializationKey(ENTITY_1A));
	EXPECT_NO_THROW(deserializer->UnregisterEntity(ENTITY_1A));
	EXPECT_FALSE(deserializer->HasSerializationKey(ENTITY_1A));

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, UnregisterEntityThrows)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(ENTITY_1A));
	EXPECT_THROW(deserializer->UnregisterEntity(ENTITY_1A), std::runtime_error);

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, GenerateEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	std::unique_ptr<ISerializableEntity> entity = deserializer->GenerateEntity(ENTITY_1A);
	EXPECT_TRUE(entity);
	EXPECT_TRUE(entity->GetKey() == ENTITY_1A);

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, GenerateEntityThrows)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(ENTITY_1A));
	EXPECT_THROW(deserializer->GenerateEntity(ENTITY_1A), std::runtime_error);

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserializerFixture, DeserializeRoot)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(deserializer->Deserialize(entity1a));

	// verify
	SerializationMembersMap membersMap = entity1a.GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.size() == size_t(1));
	auto entity2a = std::static_pointer_cast<TypeA>(entity1a.GetAggregatedProtectedMember(ENTITY_2A));
	EXPECT_TRUE(entity2a);
	EXPECT_TRUE(entity2a->GetKey() == ENTITY_2A);

	membersMap = entity2a->GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.size() == size_t(1));
	auto entity1b = std::static_pointer_cast<TypeA>(entity2a->GetAggregatedProtectedMember(ENTITY_1B));
	EXPECT_TRUE(entity1b);
	EXPECT_TRUE(entity1b->GetKey() == ENTITY_1B);

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserializerFixture, DeserializeIntermediate)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity2a;
	entity2a.SetKey(ENTITY_2A);
	EXPECT_NO_THROW(deserializer->Deserialize(entity2a));

	// verify
	SerializationMembersMap membersMap = entity2a.GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.size() == size_t(1));
	auto entity1b = std::static_pointer_cast<TypeA>(entity2a.GetAggregatedProtectedMember(ENTITY_1B));
	EXPECT_TRUE(entity1b);
	EXPECT_TRUE(entity1b->GetKey() == ENTITY_1B);

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserializerFixture, DeserializeLeaf)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity1b;
	entity1b.SetKey(ENTITY_1B);
	EXPECT_NO_THROW(deserializer->Deserialize(entity1b));

	// verify
	SerializationMembersMap membersMap = entity1b.GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.empty());

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, DeserializeThrowsWithoutSerializationStructure)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_THROW(deserializer->Deserialize(TypeA()), std::runtime_error);

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserializerFixture, DeserializeThrowsWithBadKey)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	TypeA badEntity;
	badEntity.SetKey(BAD_KEY);
	EXPECT_THROW(deserializer->Deserialize(badEntity), std::runtime_error);

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserializerFixture, LazyLoadEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// Create entity with unloaded member
	std::shared_ptr<TypeA> entity = CreateEntityWithUnloadedMember(ENTITY_1A, ENTITY_2A);
	SerializationMembersMap members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(members.find(ENTITY_2A) == members.cend());

	// try to lazy load the entity
	SharedEntity lazyLoaded = entity->GetAggregatedProtectedMember(ENTITY_2A);
	members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(&(*members[ENTITY_2A]) == &(*lazyLoaded));

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserializerFixture, LazyLoadEntityWithoutSerialization)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// Create entity with unloaded member
	TypeA entity;
	entity.AggregateProtectedMember(BAD_KEY);

	// try to lazy load the entity
	SharedEntity lazyLoaded = entity.GetAggregatedProtectedMemberPtr(BAD_KEY);
	EXPECT_TRUE(lazyLoaded == nullptr);

	EntityAggregationDeserializer::ResetInstance();
}