#include "config.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Resources/Resource.h"
#include "Resources/ResourceSerializer.h"

namespace fs = std::filesystem;

namespace
{
const std::string RESOURCE_ROOT = "C:/users/miclomba/Downloads"; 
const std::string RESOURCE_KEY = "resource";
const fs::path RESOURCE_FILE = fs::path(RESOURCE_ROOT) / (RESOURCE_KEY + ".bin");
const std::vector<int> INT_VALUES(1, 1);

class ContainerResource : public resource::Resource<std::vector<int>>
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
	EXPECT_NO_THROW(resource::ResourceSerializer::GetInstance());
	auto serializer = resource::ResourceSerializer::GetInstance();
	EXPECT_TRUE(serializer);
	EXPECT_NO_THROW(resource::ResourceSerializer::ResetInstance());
}

TEST(ResourceSerializer, ResetInstance)
{
	EXPECT_NO_THROW(resource::ResourceSerializer::ResetInstance());
}

TEST(ResourceSerializer, SetSerializationPath)
{
	resource::ResourceSerializer* serializer = resource::ResourceSerializer::GetInstance();

	EXPECT_NO_THROW(serializer->SetSerializationPath(RESOURCE_ROOT));

	resource::ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, GetSerializationPath)
{
	resource::ResourceSerializer* serializer = resource::ResourceSerializer::GetInstance();

	serializer->SetSerializationPath(RESOURCE_ROOT);
	EXPECT_EQ(serializer->GetSerializationPath(), RESOURCE_ROOT);

	resource::ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, Serialize)
{
	resource::ResourceSerializer* serializer = resource::ResourceSerializer::GetInstance();

	serializer->SetSerializationPath(RESOURCE_ROOT);

	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	// serialize
	ContainerResource resource(INT_VALUES);
	EXPECT_NO_THROW(serializer->Serialize(resource, RESOURCE_KEY));

	// verify serialization and clean up
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	resource::ResourceSerializer::ResetInstance();
}

TEST(ResourceSerializer, ThrowOnSerializeWithoutSerializationPath)
{
	resource::ResourceSerializer* serializer = resource::ResourceSerializer::GetInstance();

	ContainerResource resource(INT_VALUES);
	EXPECT_THROW(serializer->Serialize(resource, RESOURCE_KEY), std::runtime_error);

	resource::ResourceSerializer::ResetInstance();
}

