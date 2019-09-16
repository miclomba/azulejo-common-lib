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

using entity::EntityAggregationDeserializer;
using entity::ISerializableEntity;
using Key = entity::Entity::Key;
using SharedEntity = entity::Entity::SharedEntity;
using SerializationMembersMap = entity::ISerializableEntity::SerializableMemberMap;

namespace
{
const std::string JSON_ROOT = "C:/users/miclomba/Downloads"; 
const std::string JSON_FILE = "test.json";
const std::string ENTITY_1A = "entity_1a";
const std::string ENTITY_2A = "entity_2a";
const std::string ENTITY_1B = "entity_1b";
const std::string BAD_KEY = "bad_key";
const std::string VALUE = "value";

class TypeA : public ISerializableEntity
{
public:
	TypeA() = default;
	~TypeA()
	{
		if (!path_.empty())
			fs::remove(path_);
	}

	void AggregateProtectedMember(SharedEntity entity) { AggregateMember(std::move(entity)); };
	void AggregateProtectedMember(const Key& key) { AggregateMember(key); };

	const SerializationMembersMap GetAggregatedProtectedMembers() { return GetAggregatedMembers(); };
	SharedEntity GetAggregatedProtectedMember(const Key& key) { return GetAggregatedMember(key); }
	SharedEntity GetAggregatedProtectedMemberPtr(const Key& key) { return GetAggregatedMember(key); }
	const std::vector<Key> GetAggregatedProtectedMemberKeys() const { return GetAggregatedMemberKeys(); }

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
		for (std::string& dir : directoryList_)
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

	std::string jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));

	EXPECT_THROW(deserializer->LoadSerializationStructure(jsonFile), std::runtime_error);

	CreateEntityFile(jsonFile);
	EXPECT_TRUE(fs::exists(jsonFile));

	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(jsonFile));

	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));

	EntityAggregationDeserializer::ResetInstance();
}

TEST(EntityAggregationDeserializer, HasSerializationStructure)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	std::string jsonFile = (fs::path(JSON_ROOT) / JSON_FILE).string();
	EXPECT_FALSE(fs::exists(jsonFile));
	CreateEntityFile(jsonFile);
	EXPECT_TRUE(fs::exists(jsonFile));

	EXPECT_FALSE(deserializer->HasSerializationStructure());
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(jsonFile));
	EXPECT_TRUE(deserializer->HasSerializationStructure());

	fs::remove(jsonFile);
	EXPECT_FALSE(fs::exists(jsonFile));

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, GenerateEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	std::unique_ptr<ISerializableEntity> entity = deserializer->GenerateEntity(ENTITY_1A);
	EXPECT_TRUE(entity);

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, RegisterEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterEntity<TypeA>(ENTITY_1A));

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, ThrowOnRegisterEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_THROW(deserializer->RegisterEntity<TypeA>(""), std::runtime_error);

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, UnregisterEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterEntity<TypeA>(ENTITY_1A));
	EXPECT_NO_THROW(deserializer->UnregisterEntity(ENTITY_1A));

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, ThrowOnUnregisterEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	EXPECT_THROW(deserializer->UnregisterEntity(ENTITY_1A), std::runtime_error);

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, DeserializeRoot)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(TypeA()), std::runtime_error);

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(deserializer->Deserialize(entity1a));

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, DeserializeIntermediate)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(TypeA()), std::runtime_error);

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity2a;
	entity2a.SetKey(ENTITY_2A);
	EXPECT_NO_THROW(deserializer->Deserialize(entity2a));

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, DeserializeLeaf)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(TypeA()), std::runtime_error);

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity1b;
	entity1b.SetKey(ENTITY_1B);
	EXPECT_NO_THROW(deserializer->Deserialize(entity1b));

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, DeserializeBadKey)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(TypeA()), std::runtime_error);

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA badEntity;
	badEntity.SetKey(BAD_KEY);
	EXPECT_THROW(deserializer->Deserialize(badEntity), std::runtime_error);

	EntityAggregationDeserializer::ResetInstance();
}

TEST_F(EntityAggregationDeserialization, LazyLoadEntity)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// try to deserialize without a serialization structure
	EXPECT_THROW(deserializer->Deserialize(TypeA()), std::runtime_error);

	// load serialization structure
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

TEST_F(EntityAggregationDeserialization, LazyLoadEntityWithoutSerialization)
{
	EntityAggregationDeserializer* deserializer = EntityAggregationDeserializer::GetInstance();

	// register deserialization types
	deserializer->RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->RegisterEntity<TypeA>(ENTITY_1B);

	// load serialization structure
	EXPECT_NO_THROW(deserializer->LoadSerializationStructure(GetJSONFile()));

	// Create entity with unloaded member
	TypeA entity;
	entity.AggregateProtectedMember(BAD_KEY);

	// try to lazy load the entity
	SharedEntity lazyLoaded = entity.GetAggregatedProtectedMemberPtr(BAD_KEY);
	EXPECT_TRUE(lazyLoaded == nullptr);

	EntityAggregationDeserializer::ResetInstance();
}