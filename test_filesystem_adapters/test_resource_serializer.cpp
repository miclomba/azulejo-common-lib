#include "config.h"

#include <stdexcept>
#include <string>
#include <vector>
#include "Config/filesystem.hpp"

#include <gtest/gtest.h>

#include "ContainerResource.h"
#include "ContainerResource2D.h"
#include "FilesystemAdapters/ResourceSerializer.h"

using filesystem_adapters::ResourceSerializer;
using Resource = ContainerResource<int>;
using Resource2D = ContainerResource2D<int>;

namespace
{
const std::string RESOURCE_ROOT = (fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY).string(); 
const std::string RESOURCE_KEY = "resource";
const fs::path RESOURCE_FILE = fs::path(RESOURCE_ROOT) / (RESOURCE_KEY + ".bin");
const std::vector<std::vector<int>> INT_VALUES(1, std::vector<int>(1,1));
const std::vector<int> INT_VALUES_ARRAY(1,1);
} // end namespace 

TEST(ResourceSerializer, GetInstance)
{
	EXPECT_NO_THROW(ResourceSerializer::GetInstance());
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();
	EXPECT_TRUE(serializer);
	EXPECT_NO_THROW(ResourceSerializer::ResetInstance());
}

TEST(ResourceSerializer, ResetInstance)
{
	EXPECT_NO_THROW(ResourceSerializer::ResetInstance());
}

TEST(ResourceSerializer, SetSerializationPath)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(RESOURCE_ROOT));

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, GetSerializationPath)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	serializer->SetSerializationPath(RESOURCE_ROOT);
	EXPECT_EQ(serializer->GetSerializationPath(), RESOURCE_ROOT);

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, GetSerializationPathThrows)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	EXPECT_THROW(serializer->GetSerializationPath(), std::runtime_error);

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, SerializeArray)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	serializer->SetSerializationPath(RESOURCE_ROOT);

	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	// serialize
	Resource resource(INT_VALUES_ARRAY);
	EXPECT_NO_THROW(serializer->Serialize(resource, RESOURCE_KEY));

	// verify serialization and clean up
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, SerializeMatrix)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	serializer->SetSerializationPath(RESOURCE_ROOT);

	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	// serialize
	Resource2D resource(INT_VALUES);
	EXPECT_NO_THROW(serializer->Serialize(resource, RESOURCE_KEY));

	// verify serialization and clean up
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, SerializeThrowsUsingEmptyKey)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	serializer->SetSerializationPath(RESOURCE_ROOT);

	Resource2D resource(INT_VALUES);
	EXPECT_THROW(serializer->Serialize(resource, ""), std::runtime_error);

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, SerializeThrowsWithoutSerializationPath)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	Resource2D resource(INT_VALUES);
	EXPECT_THROW(serializer->Serialize(resource, RESOURCE_KEY), std::runtime_error);

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, Unserialize)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	serializer->SetSerializationPath(RESOURCE_ROOT);

	Resource resource(INT_VALUES_ARRAY);
	serializer->Serialize(resource, RESOURCE_KEY);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));

	serializer->Unserialize(RESOURCE_KEY);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, UnserializeThrowsWithEmptyKey)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	EXPECT_THROW(serializer->Unserialize(""), std::runtime_error);

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, UnserializeDoesNotThrowsWithUnserializedKey)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	serializer->SetSerializationPath(RESOURCE_ROOT);
	EXPECT_NO_THROW(serializer->Unserialize(RESOURCE_KEY));

	ResourceSerializer::ResetInstance();
}


