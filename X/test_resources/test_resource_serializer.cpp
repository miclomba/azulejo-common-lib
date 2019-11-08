#include "config.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Resources/Resource.h"
#include "Resources/ResourceSerializer.h"

namespace fs = std::filesystem;

using resource::Resource;
using resource::ResourceSerializer;

namespace
{
const std::string RESOURCE_ROOT = (fs::path(ROOT_FILESYSTEM) / "users" / "miclomba" / "Downloads").string(); 
const std::string RESOURCE_KEY = "resource";
const fs::path RESOURCE_FILE = fs::path(RESOURCE_ROOT) / (RESOURCE_KEY + ".bin");
const std::vector<std::vector<int>> INT_VALUES(1, std::vector<int>(1,1));

class ContainerResource : public Resource<int>
{
public:
	ContainerResource(std::vector<std::vector<int>>&& values) : Resource(std::move(values)) {}
	ContainerResource(const std::vector<std::vector<int>>& values) : Resource(values) {}
};
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

TEST(ResourceSerializer, Serialize)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	serializer->SetSerializationPath(RESOURCE_ROOT);

	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	// serialize
	ContainerResource resource(INT_VALUES);
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

	ContainerResource resource(INT_VALUES);
	EXPECT_THROW(serializer->Serialize(resource, ""), std::runtime_error);

	ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, SerializeThrowsWithoutSerializationPath)
{
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	ContainerResource resource(INT_VALUES);
	EXPECT_THROW(serializer->Serialize(resource, RESOURCE_KEY), std::runtime_error);

	ResourceSerializer::ResetInstance();
}

