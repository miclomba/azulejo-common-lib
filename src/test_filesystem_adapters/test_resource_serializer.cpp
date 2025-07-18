#include "test_filesystem_adapters/config.h"

#include <stdexcept>
#include <string>
#include <vector>
#include "Config/filesystem.hpp"

#include <gtest/gtest.h>

#include "test_filesystem_adapters/ContainerResource.h"
#include "test_filesystem_adapters/ContainerResource2D.h"
#include "FilesystemAdapters/ResourceSerializer.h"

using filesystem_adapters::ResourceSerializer;
using Resource = ContainerResource<int>;
using Resource2D = ContainerResource2D<int>;

namespace
{
	const std::string RESOURCE_ROOT = (fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY).string();
	const std::string RESOURCE_KEY = "resource";
	const fs::path RESOURCE_FILE = fs::path(RESOURCE_ROOT) / (RESOURCE_KEY + ".bin");
	const std::vector<std::vector<int>> INT_VALUES(1, std::vector<int>(1, 1));
	const std::vector<int> INT_VALUES_ARRAY(1, 1);
} // end namespace

TEST(ResourceSerializer, GetInstance)
{
	EXPECT_NO_THROW(ResourceSerializer::GetInstance());
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();
	EXPECT_TRUE(serializer);
}

TEST(ResourceSerializer, SerializeArray)
{
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	// serialize
	Resource resource(INT_VALUES_ARRAY);
	EXPECT_NO_THROW(serializer->Serialize(resource, RESOURCE_KEY, RESOURCE_ROOT));

	// verify serialization and clean up
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
}

TEST(ResourceSerializer, SerializeMatrix)
{
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	// serialize
	Resource2D resource(INT_VALUES);
	EXPECT_NO_THROW(serializer->Serialize(resource, RESOURCE_KEY, RESOURCE_ROOT));

	// verify serialization and clean up
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
}

TEST(ResourceSerializer, SerializeThrowsUsingEmptyKey)
{
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	Resource2D resource(INT_VALUES);
	EXPECT_THROW(serializer->Serialize(resource, "", RESOURCE_ROOT), std::runtime_error);
}

TEST(ResourceSerializer, SerializeThrowsWithoutSerializationPath)
{
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	Resource2D resource(INT_VALUES);
	EXPECT_THROW(serializer->Serialize(resource, RESOURCE_KEY, ""), std::runtime_error);
}

TEST(ResourceSerializer, Unserialize)
{
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	Resource resource(INT_VALUES_ARRAY);
	serializer->Serialize(resource, RESOURCE_KEY, RESOURCE_ROOT);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));

	serializer->Unserialize(RESOURCE_KEY, RESOURCE_ROOT);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
}

TEST(ResourceSerializer, UnserializeThrowsWithEmptyKey)
{
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	EXPECT_THROW(serializer->Unserialize("", RESOURCE_ROOT), std::runtime_error);
}

TEST(ResourceSerializer, UnserializeDoesNotThrowsWithUnserializedKey)
{
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->Unserialize(RESOURCE_KEY, RESOURCE_ROOT));
}
