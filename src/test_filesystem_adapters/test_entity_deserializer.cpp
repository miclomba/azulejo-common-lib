#include "test_filesystem_adapters/config.h"

#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "Config/filesystem.hpp"

#include <gtest/gtest.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "FilesystemAdapters/ISerializableEntity.h"
#include "FilesystemAdapters/EntityDeserializer.h"

namespace pt = boost::property_tree;

using entity::Entity;
using filesystem_adapters::EntityDeserializer;
using filesystem_adapters::ISerializableEntity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
	const std::string JSON_ROOT = (fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY).string();
	const std::string JSON_FILE = "test.json";
	const std::string ENTITY_1A = "entity_1a";
	const std::string ENTITY_2A = "entity_2a";
	const std::string ENTITY_1B = "entity_1b";
	const std::string NON_ENTITY = "not_an_entity";
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

		void AggregateProtectedMember(const Key &key) { AggregateMember(key); };

		std::map<Key, ISerializableEntity *> GetAggregatedProtectedMembers()
		{
			std::map<Key, ISerializableEntity *> serializableMemberMap;

			std::vector<std::string> keys = GetAggregatedMemberKeys<ISerializableEntity>();
			for (const std::string &key : keys)
			{
				Entity::SharedEntity &member = GetAggregatedMember(key);
				auto memberPtr = dynamic_cast<ISerializableEntity *>(member.get());
				serializableMemberMap.insert(std::make_pair(key, memberPtr));
			}
			return serializableMemberMap;
		};

		SharedEntity GetAggregatedProtectedMember(const Key &key) { return GetAggregatedMember(key); }
		SharedEntity GetAggregatedProtectedMemberPtr(const Key &key) { return GetAggregatedMember(key); }

		void Save(pt::ptree &tree, const std::string &path) const override
		{
			path_ = path;
			fs::create_directories(path);
		}

		void Load(pt::ptree &tree, const std::string &path) override
		{
			EXPECT_TRUE(fs::exists(path));
		}

		std::string GetPath() const
		{
			return path_;
		}

		void SetPath(const std::string &p)
		{
			path_ = p;
		}

	private:
		mutable std::string path_;
	};

	std::vector<std::string> CreateEntityFile(const std::string &filePath)
	{
		std::vector<std::string> directories;

		pt::ptree root;
		root.add_child(ENTITY_1A, pt::ptree());
		directories.push_back(ENTITY_1A);

		pt::ptree &child = root.get_child(ENTITY_1A);
		child.add_child(ENTITY_2A, pt::ptree());
		directories.push_back((fs::path(ENTITY_1A) / ENTITY_2A).string());

		pt::ptree &child2 = child.get_child(ENTITY_2A);
		child2.add_child(ENTITY_1B, pt::ptree(VALUE));
		// add a child that won't have a corressponding entity
		child2.add_child(NON_ENTITY, pt::ptree(VALUE));
		directories.push_back((fs::path(ENTITY_1A) / ENTITY_2A / ENTITY_1B).string());

		pt::json_parser::write_json(filePath, root);

		return directories;
	}

	std::shared_ptr<TypeA> CreateEntityWithUnloadedMember(const Key &root, const Key &leaf)
	{
		auto rootEntity = std::make_shared<TypeA>();

		rootEntity->SetKey(root);

		rootEntity->AggregateProtectedMember(leaf);

		return rootEntity;
	}

	struct EntityDeserializerF : public testing::Test
	{
		EntityDeserializerF()
		{
			// create serialization structure
			jsonFile_ = (fs::path(JSON_ROOT) / JSON_FILE).string();
			EXPECT_FALSE(fs::exists(jsonFile_));
			directoryList_ = CreateEntityFile(jsonFile_);
			EXPECT_TRUE(fs::exists(jsonFile_));

			// create a serialization
			for (std::string &dir : directoryList_)
			{
				fs::path d = fs::path(JSON_ROOT) / dir;
				EXPECT_FALSE(fs::exists(d));
				fs::create_directories(d);
				EXPECT_TRUE(fs::exists(d));
			}
		}

		~EntityDeserializerF()
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

TEST(EntityDeserializer, GetInstance)
{
	EXPECT_NO_THROW(EntityDeserializer::GetInstance());
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();
	EXPECT_TRUE(deserializer);
	EXPECT_NO_THROW(EntityDeserializer::ResetInstance());
}

TEST(EntityDeserializer, ResetInstance)
{
	EXPECT_NO_THROW(EntityDeserializer::ResetInstance());
}

TEST_F(EntityDeserializerF, DeserializeRoot)
{
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();

	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->GetHierarchy().LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity1a;
	entity1a.SetKey(ENTITY_1A);
	EXPECT_NO_THROW(deserializer->LoadEntity(entity1a));

	// verify
	std::map<Key, ISerializableEntity *> membersMap = entity1a.GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.size() == size_t(1));
	auto entity2a = std::dynamic_pointer_cast<TypeA>(entity1a.GetAggregatedProtectedMember(ENTITY_2A));
	EXPECT_TRUE(entity2a);
	EXPECT_TRUE(entity2a->GetKey() == ENTITY_2A);

	membersMap = entity2a->GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.size() == size_t(1));
	auto entity1b = std::dynamic_pointer_cast<TypeA>(entity2a->GetAggregatedProtectedMember(ENTITY_1B));
	EXPECT_TRUE(entity1b);
	EXPECT_TRUE(entity1b->GetKey() == ENTITY_1B);

	EntityDeserializer::ResetInstance();
}

TEST_F(EntityDeserializerF, DeserializeIntermediate)
{
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();

	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->GetHierarchy().LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity2a;
	entity2a.SetKey(ENTITY_2A);
	EXPECT_NO_THROW(deserializer->LoadEntity(entity2a));

	// verify
	std::map<Key, ISerializableEntity *> membersMap = entity2a.GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.size() == size_t(1));
	auto entity1b = std::dynamic_pointer_cast<TypeA>(entity2a.GetAggregatedProtectedMember(ENTITY_1B));
	EXPECT_TRUE(entity1b);
	EXPECT_TRUE(entity1b->GetKey() == ENTITY_1B);

	EntityDeserializer::ResetInstance();
}

TEST_F(EntityDeserializerF, DeserializeLeaf)
{
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();

	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->GetHierarchy().LoadSerializationStructure(GetJSONFile()));

	// deserialize an entity
	TypeA entity1b;
	entity1b.SetKey(ENTITY_1B);
	EXPECT_NO_THROW(deserializer->LoadEntity(entity1b));

	// verify
	std::map<Key, ISerializableEntity *> membersMap = entity1b.GetAggregatedProtectedMembers();
	EXPECT_TRUE(membersMap.empty());

	EntityDeserializer::ResetInstance();
}

TEST(EntityDeserializer, DeserializeReturnsWithoutSerializationStructure)
{
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();

	TypeA typeA;
	typeA.SetKey(BAD_KEY);
	typeA.SetPath(VALUE);

	EXPECT_NO_THROW(deserializer->LoadEntity(typeA));
	EXPECT_EQ(typeA.GetPath(), VALUE);
	EXPECT_EQ(typeA.GetKey(), BAD_KEY);

	EntityDeserializer::ResetInstance();
}

TEST_F(EntityDeserializerF, DeserializeReturnsWithBadKey)
{
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->GetHierarchy().LoadSerializationStructure(GetJSONFile()));

	TypeA badEntity;
	badEntity.SetKey(BAD_KEY);
	badEntity.SetPath(VALUE);

	EXPECT_NO_THROW(deserializer->LoadEntity(badEntity));
	EXPECT_EQ(badEntity.GetPath(), VALUE);
	EXPECT_EQ(badEntity.GetKey(), BAD_KEY);

	EntityDeserializer::ResetInstance();
}

TEST_F(EntityDeserializerF, LazyLoadEntity)
{
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();

	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->GetHierarchy().LoadSerializationStructure(GetJSONFile()));

	// Create entity with unloaded member
	std::shared_ptr<TypeA> entity = CreateEntityWithUnloadedMember(ENTITY_1A, ENTITY_2A);
	std::map<Key, ISerializableEntity *> members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(members.find(ENTITY_2A) == members.cend());

	// try to lazy load the entity
	SharedEntity lazyLoaded = entity->GetAggregatedProtectedMember(ENTITY_2A);
	members = entity->GetAggregatedProtectedMembers();
	EXPECT_TRUE(&(*members[ENTITY_2A]) == &(*lazyLoaded));

	EntityDeserializer::ResetInstance();
}

TEST_F(EntityDeserializerF, LazyLoadEntityWithoutSerialization)
{
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();

	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_2A);
	deserializer->GetRegistry().RegisterEntity<TypeA>(ENTITY_1B);

	EXPECT_NO_THROW(deserializer->GetHierarchy().LoadSerializationStructure(GetJSONFile()));

	// Create entity with unloaded member
	TypeA entity;
	entity.AggregateProtectedMember(BAD_KEY);

	// try to lazy load the entity
	SharedEntity lazyLoaded = entity.GetAggregatedProtectedMemberPtr(BAD_KEY);
	EXPECT_TRUE(lazyLoaded == nullptr);

	EntityDeserializer::ResetInstance();
}

TEST(EntityDeserializer, GetRegistry)
{
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->GetRegistry());

	EntityDeserializer::ResetInstance();
}

TEST(EntityDeserializer, GetHierarchy)
{
	EntityDeserializer *deserializer = EntityDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->GetHierarchy());

	EntityDeserializer::ResetInstance();
}