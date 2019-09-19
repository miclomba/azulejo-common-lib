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
const std::string RESOURCE_ROOT = "C:/users/miclomba/Downloads"; 
const std::string RESOURCE_KEY = "resource";
const fs::path RESOURCE_FILE = fs::path(RESOURCE_ROOT) / (RESOURCE_KEY + ".bin");
const std::vector<int> INT_VALUES(1, 1);

class ContainerResource : public Resource<std::vector<int>>
{
public:
	ContainerResource(std::vector<int>&& values) : Resource(std::move(values)) {}
	ContainerResource(const std::vector<int>& values) : Resource(values) {}
	std::vector<int> GetData() const { return Data(); }
	void SetData(const std::vector<int>& values) { Data() = values; }
	bool IsDirtyProtected() { return IsDirty(); }
	int ChecksumProtected() { return Checksum(); }
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

