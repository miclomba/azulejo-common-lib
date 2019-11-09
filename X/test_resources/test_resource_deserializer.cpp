#include "config.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Resources/Resource.h"
#include "Resources/ResourceDeserializer.h"
#include "Resources/ResourceSerializer.h"

namespace fs = std::filesystem;

using resource::IResource;
using resource::Resource;
using resource::ResourceDeserializer;
using resource::ResourceSerializer;

namespace
{
const std::string RESOURCE_ROOT = (fs::path(ROOT_FILESYSTEM) / "users" / "miclomba" / "Downloads").string(); 
const std::string RESOURCE_KEY = "resource";
const std::string BAD_PATH = "$helloworld$";
const fs::path RESOURCE_FILE = fs::path(RESOURCE_ROOT) / (RESOURCE_KEY + ".bin");
const std::vector<std::vector<int>> INT_VALUES(1, std::vector<int>(1,1));

class ContainerResource : public Resource<int>
{
public:
	ContainerResource() : Resource() {};
	ContainerResource(std::vector<std::vector<int>>&& values) : Resource(std::move(values)) {}
	ContainerResource(const std::vector<std::vector<int>>& values) : Resource(values) {}
};
} // end namespace 

TEST(ResourceDeserializer, GetInstance)
{
	EXPECT_NO_THROW(ResourceDeserializer::GetInstance());
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();
	EXPECT_TRUE(deserializer);
	EXPECT_NO_THROW(ResourceDeserializer::ResetInstance());
}

TEST(ResourceDeserializer, ResetInstance)
{
	EXPECT_NO_THROW(ResourceDeserializer::ResetInstance());
}

TEST(ResourceDeserializer, SetSerializationPath)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->SetSerializationPath(RESOURCE_ROOT));

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, GetSerializationPath)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	deserializer->SetSerializationPath(RESOURCE_ROOT);
	EXPECT_EQ(deserializer->GetSerializationPath(), RESOURCE_ROOT);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, GetSerializationPathThrows)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->GetSerializationPath(), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, RegisterResource)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY));

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, RegisterResourceThrowsOnEmptyKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->RegisterResource<int>(""), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, RegisterResourceThrowsOnExistingKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY));
	EXPECT_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, UnregisterResource)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY));
	EXPECT_NO_THROW(deserializer->UnregisterResource(RESOURCE_KEY));

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, UnregisterResourceThrowsOnEmptyKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->UnregisterResource(""), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, UnregisterResourceThrowsOnUnregisteredKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->UnregisterResource(RESOURCE_KEY), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, UnregisterAll)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(RESOURCE_KEY));
	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY));
	EXPECT_TRUE(deserializer->HasSerializationKey(RESOURCE_KEY));
	EXPECT_NO_THROW(deserializer->UnregisterAll());
	EXPECT_FALSE(deserializer->HasSerializationKey(RESOURCE_KEY));

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, HasSerializationKeyFalse)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(RESOURCE_KEY));

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, HasSerializationKeyTrue)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY));
	EXPECT_TRUE(deserializer->HasSerializationKey(RESOURCE_KEY));

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, GenerateResource)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<int>(RESOURCE_KEY);
	std::unique_ptr<IResource> resource = deserializer->GenerateResource(RESOURCE_KEY);
	EXPECT_TRUE(resource);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, GenerateResourceThrowsOnEmptyKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->GenerateResource(""), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, GenerateResourceThrowsOnUnregisteredKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->GenerateResource(RESOURCE_KEY), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, Deserialize)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	deserializer->SetSerializationPath(RESOURCE_ROOT);
	serializer->SetSerializationPath(RESOURCE_ROOT);

	// serialize
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
	ContainerResource resource(INT_VALUES);
	serializer->Serialize(resource, RESOURCE_KEY);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));
	
	// deserialize
	deserializer->RegisterResource<int>(RESOURCE_KEY);
	std::unique_ptr<IResource> rsrc = deserializer->Deserialize(RESOURCE_KEY);
	auto vecIntResource = static_cast<Resource<int>*>(rsrc.get());
	EXPECT_TRUE(vecIntResource);
	EXPECT_EQ(vecIntResource->Data(), resource.Data());
	EXPECT_EQ(vecIntResource->GetColumnSize(), resource.GetColumnSize());
	EXPECT_EQ(vecIntResource->GetRowSize(), resource.GetRowSize());

	// clean up
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	ResourceDeserializer::ResetInstance();
	ResourceSerializer::ResetInstance();
}

TEST(ResourceDeserializer, DeserializeEmptyResource)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();
	ResourceSerializer* serializer = ResourceSerializer::GetInstance();

	deserializer->SetSerializationPath(RESOURCE_ROOT);
	serializer->SetSerializationPath(RESOURCE_ROOT);

	// serialize
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
	ContainerResource resource;
	serializer->Serialize(resource, RESOURCE_KEY);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));

	// deserialize
	deserializer->RegisterResource<int>(RESOURCE_KEY);
	std::unique_ptr<IResource> rsrc = deserializer->Deserialize(RESOURCE_KEY);
	auto vecIntResource = static_cast<Resource<int>*>(rsrc.get());
	EXPECT_TRUE(vecIntResource);
	EXPECT_EQ(vecIntResource->Data(), resource.Data());
	EXPECT_EQ(vecIntResource->GetColumnSize(), resource.GetColumnSize());
	EXPECT_EQ(vecIntResource->GetRowSize(), resource.GetRowSize());

	// clean up
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	ResourceDeserializer::ResetInstance();
	ResourceSerializer::ResetInstance();
}

TEST(ResourceDeserializer, DeserializeThrowsWithoutSerializationPath)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<int>(RESOURCE_KEY);
	EXPECT_THROW(deserializer->Deserialize(RESOURCE_KEY), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, DeserializeThrowsWithBadSerializationPath)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	deserializer->SetSerializationPath(BAD_PATH);
	deserializer->RegisterResource<int>(RESOURCE_KEY);
	EXPECT_THROW(deserializer->Deserialize(RESOURCE_KEY), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, DeserializeThrowsWithEmptyKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	deserializer->SetSerializationPath(RESOURCE_ROOT);
	deserializer->RegisterResource<int>(RESOURCE_KEY);
	EXPECT_THROW(deserializer->Deserialize(""), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

