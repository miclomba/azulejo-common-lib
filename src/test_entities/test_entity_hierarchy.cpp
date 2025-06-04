#include "test_entities/config.h"

#include <stdexcept>
#include <string>
#include "Config/filesystem.hpp"

#include <gtest/gtest.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"

namespace pt = boost::property_tree;

using entity::EntityHierarchy;

namespace
{
	const fs::path JSON_ROOT = fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY;
	const std::string JSON_FILE = "test.json";
	const std::string ENTITY_1A = "entity_1a";

	void CreateEntityFile(const fs::path &filePath)
	{
		pt::ptree root;
		root.add_child(ENTITY_1A, pt::ptree());
		pt::json_parser::write_json(filePath.string(), root);
	}

	fs::path CreateJSONFile()
	{
		fs::path jsonFile = fs::path(JSON_ROOT) / JSON_FILE;
		CreateEntityFile(jsonFile);
		EXPECT_TRUE(fs::exists(jsonFile));
		return jsonFile;
	}

	void RemoveJSONFile(const fs::path &jsonFile)
	{
		fs::remove(jsonFile);
		EXPECT_FALSE(fs::exists(jsonFile));
	}

} // end namespace anonymous

TEST(EntityHierarchy, LoadSerializationStructure)
{
	EntityHierarchy hierarchy;
	fs::path jsonFile = CreateJSONFile();

	EXPECT_NO_THROW(hierarchy.LoadSerializationStructure(jsonFile));

	RemoveJSONFile(jsonFile);
}

TEST(EntityHierarchy, LoadSerializationStructureThrows)
{
	EntityHierarchy hierarchy;
	fs::path jsonFile = fs::path(JSON_ROOT) / JSON_FILE;
	EXPECT_FALSE(fs::exists(jsonFile));

	EXPECT_THROW(hierarchy.LoadSerializationStructure(jsonFile), std::runtime_error);
}

TEST(EntityHierarchy, HasSerializationStructure)
{
	EntityHierarchy hierarchy;
	fs::path jsonFile = CreateJSONFile();

	EXPECT_FALSE(hierarchy.HasSerializationStructure());
	EXPECT_NO_THROW(hierarchy.LoadSerializationStructure(jsonFile));
	EXPECT_TRUE(hierarchy.HasSerializationStructure());

	RemoveJSONFile(jsonFile);
}

TEST(EntityHierarchy, GetSerializationStructure)
{
	EntityHierarchy hierarchy;
	fs::path jsonFile = CreateJSONFile();
	EXPECT_NO_THROW(hierarchy.LoadSerializationStructure(jsonFile));

	EXPECT_NO_THROW(hierarchy.GetSerializationStructure());
	pt::ptree structure = hierarchy.GetSerializationStructure();
	EXPECT_TRUE(!structure.empty());

	RemoveJSONFile(jsonFile);
}

TEST(EntityHierarchy, SetSerializationPath)
{
	EntityHierarchy hierarchy;
	EXPECT_NO_THROW(hierarchy.SetSerializationPath(JSON_ROOT));
	EXPECT_EQ(hierarchy.GetSerializationPath(), JSON_ROOT);
}

TEST(EntityHierarchy, GetSerializationPath)
{
	EntityHierarchy hierarchy;
	EXPECT_NO_THROW(hierarchy.SetSerializationPath(JSON_ROOT));
	EXPECT_EQ(hierarchy.GetSerializationPath(), JSON_ROOT);
}
