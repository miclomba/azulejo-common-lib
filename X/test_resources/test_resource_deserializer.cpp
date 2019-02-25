#include "config.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Resources/Resource.h"
#include "Resources/ResourceDeserializer.h"
#include "Resources/ResourceSerializer.h"

namespace fs = std::filesystem;
typedef std::vector<int> VecInt;

namespace
{
const std::string RESOURCE_ROOT = "C:/users/miclomba/Downloads"; 
const std::string RESOURCE_KEY = "resource";
const fs::path RESOURCE_FILE = fs::path(RESOURCE_ROOT) / (RESOURCE_KEY + ".bin");
const std::vector<int> INT_VALUES(1, 1);

class ContainerResource : public resource::Resource<VecInt>
{
public:
	ContainerResource(VecInt&& values) : Resource(std::move(values)) {}
	ContainerResource(const VecInt& values) : Resource(values) {}
	bool IsDirtyProtected() { return IsDirty(); }
	int ChecksumProtected() { return Checksum(); }
	ContainerResource() = default;
};
} // end namespace 

TEST(ResourceDeserializer, GetInstance)
{
	EXPECT_NO_THROW(resource::ResourceDeserializer::GetInstance());
	auto deserializer = resource::ResourceDeserializer::GetInstance();
	EXPECT_TRUE(deserializer);
	EXPECT_NO_THROW(resource::ResourceDeserializer::ResetInstance());
}

TEST(ResourceDeserializer, ResetInstance)
{
	EXPECT_NO_THROW(resource::ResourceDeserializer::ResetInstance());
}

TEST(ResourceDeserializer, SetSerializationPath)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->SetSerializationPath(RESOURCE_ROOT));

	resource::ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, GetSerializationPath)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();

	deserializer->SetSerializationPath(RESOURCE_ROOT);
	EXPECT_EQ(deserializer->GetSerializationPath(), RESOURCE_ROOT);

	resource::ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, ThrowOnGetSerializationPath)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->GetSerializationPath(), std::runtime_error);

	resource::ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, GenerateResource)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<VecInt>(RESOURCE_KEY);
	auto resource = deserializer->GenerateResource(RESOURCE_KEY);
	EXPECT_TRUE(resource);

	resource::ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, RegisterResource)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<VecInt>(RESOURCE_KEY));

	resource::ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, ThrowOnRegisterResource)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->RegisterResource<VecInt>(""), std::runtime_error);

	resource::ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, UnregisterResource)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<VecInt>(RESOURCE_KEY));
	EXPECT_NO_THROW(deserializer->UnregisterResource(RESOURCE_KEY));

	resource::ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, ThrowOnUnregisterResource)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->UnregisterResource(RESOURCE_KEY), std::runtime_error);

	resource::ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, Deserialize)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();
	resource::ResourceSerializer* serializer = resource::ResourceSerializer::GetInstance();

	deserializer->SetSerializationPath(RESOURCE_ROOT);
	serializer->SetSerializationPath(RESOURCE_ROOT);

	// serialize
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
	serializer->Serialize(ContainerResource(INT_VALUES), RESOURCE_KEY);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));
	
	// deserialize
	deserializer->RegisterResource<VecInt>(RESOURCE_KEY);
	std::unique_ptr<resource::IResource> rsrc = deserializer->Deserialize(RESOURCE_KEY);
	auto vecIntResource = static_cast<resource::Resource<VecInt>*>(rsrc.get());
	EXPECT_TRUE(vecIntResource);
	EXPECT_EQ(vecIntResource->Data(), INT_VALUES);

	// clean up
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	resource::ResourceDeserializer::ResetInstance();
	resource::ResourceSerializer::ResetInstance();
}

TEST(ResourceDeserializer, ThrowOnDeserializeWithoutSerializationPath)
{
	resource::ResourceDeserializer* deserializer = resource::ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<VecInt>(RESOURCE_KEY);
	EXPECT_THROW(deserializer->Deserialize(RESOURCE_KEY), std::runtime_error);

	resource::ResourceDeserializer::ResetInstance();
}

