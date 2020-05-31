
#include "config.h"

#include <string>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include <gtest/gtest.h>

#include "DatabaseAdapters/EntityHierarchyBlob.h"

using database_adapters::EntityHierarchyBlob;
namespace pt = boost::property_tree;

namespace
{
const std::string KEY = "Key";
const std::string SERIAL_JSON = R"({
    "Key": {
    }
})";

const std::string BAD_SERIAL_JSON = "bad_json";
} // end namespace

TEST(EntityHierarchyBlob, ConstructWithLValueString)
{
	EntityHierarchyBlob hierarchy(SERIAL_JSON);

	EXPECT_EQ(hierarchy.GetColumnSize(), 1);
	EXPECT_EQ(hierarchy.GetRowSize(), SERIAL_JSON.length());
	EXPECT_EQ(hierarchy.GetElementSize(), sizeof(char));

	std::string bufferContent(reinterpret_cast<const char*>(hierarchy.Data()));
	EXPECT_EQ(bufferContent, SERIAL_JSON);
	EXPECT_TRUE(hierarchy.GetHierarchyTree().get_child_optional(KEY));
}

TEST(EntityHierarchyBlob, ConstructWithRValueString)
{
	std::string serialJSON = SERIAL_JSON;
	EntityHierarchyBlob hierarchy(std::move(serialJSON));

	EXPECT_EQ(hierarchy.GetColumnSize(), 1);
	EXPECT_EQ(hierarchy.GetRowSize(), SERIAL_JSON.length());
	EXPECT_EQ(hierarchy.GetElementSize(), sizeof(char));
	
	std::string bufferContent(reinterpret_cast<const char*>(hierarchy.Data()));
	EXPECT_EQ(bufferContent, SERIAL_JSON);
	EXPECT_TRUE(hierarchy.GetHierarchyTree().get_child_optional(KEY));
}

TEST(EntityHierarchyBlob, ConstructWithBadLValueString)
{
	EXPECT_THROW(EntityHierarchyBlob hierarchy(BAD_SERIAL_JSON), std::runtime_error);
}

TEST(EntityHierarchyBlob, ConstructWithBadRValueString)
{
	std::string serialJSON = BAD_SERIAL_JSON;
	EXPECT_THROW(EntityHierarchyBlob hierarchy(std::move(serialJSON)), std::runtime_error);
}

TEST(EntityHierarchyBlob, GetElementSize)
{
	EntityHierarchyBlob hierarchy(SERIAL_JSON);
	EXPECT_EQ(hierarchy.GetElementSize(), sizeof(char));
}

TEST(EntityHierarchyBlob, Data)
{
	EntityHierarchyBlob hierarchy(SERIAL_JSON);
	std::string bufferContent;
	
	EXPECT_NO_THROW(bufferContent = (reinterpret_cast<const char*>(hierarchy.Data())));
	EXPECT_EQ(bufferContent, SERIAL_JSON);
}

TEST(EntityHierarchyBlob, DataConst)
{
	const EntityHierarchyBlob hierarchy(SERIAL_JSON);
	std::string bufferContent;
	
	EXPECT_NO_THROW(bufferContent = (reinterpret_cast<const char*>(hierarchy.Data())));
	EXPECT_EQ(bufferContent, SERIAL_JSON);
}

TEST(EntityHierarchyBlob, Assign)
{
	EntityHierarchyBlob hierarchy;
	EXPECT_NO_THROW(hierarchy.Assign(SERIAL_JSON.data(), SERIAL_JSON.length()));

	std::string bufferContent(reinterpret_cast<const char*>(hierarchy.Data()));
	EXPECT_EQ(bufferContent, SERIAL_JSON);
	EXPECT_TRUE(hierarchy.GetHierarchyTree().get_child_optional(KEY));
}

TEST(EntityHierarchyBlob, GetHierarchyTree)
{
	EntityHierarchyBlob hierarchy(SERIAL_JSON);
	EXPECT_TRUE(hierarchy.GetHierarchyTree().get_child_optional(KEY));
}

