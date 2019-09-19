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
typedef std::vector<int> VecInt;

using resource::IResource;
using resource::Resource;
using resource::ResourceDeserializer;
using resource::ResourceSerializer;

namespace
{
const std::string RESOURCE_ROOT = "C:/users/miclomba/Downloads"; 
const std::string RESOURCE_KEY = "resource";
const std::string BAD_PATH = "$helloworld$";
const fs::path RESOURCE_FILE = fs::path(RESOURCE_ROOT) / (RESOURCE_KEY + ".bin");
const std::vector<int> INT_VALUES(1, 1);

class ContainerResource : public Resource<VecInt>
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

	EXPECT_NO_THROW(deserializer->RegisterResource<VecInt>(RESOURCE_KEY));

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, RegisterResourceThrowsOnEmptyKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->RegisterResource<VecInt>(""), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, RegisterResourceThrowsOnExistingKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<VecInt>(RESOURCE_KEY));
	EXPECT_THROW(deserializer->RegisterResource<VecInt>(RESOURCE_KEY), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, UnregisterResource)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<VecInt>(RESOURCE_KEY));
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

TEST(ResourceDeserializer, GenerateResource)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<VecInt>(RESOURCE_KEY);
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
	serializer->Serialize(ContainerResource(INT_VALUES), RESOURCE_KEY);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));
	
	// deserialize
	deserializer->RegisterResource<VecInt>(RESOURCE_KEY);
	std::unique_ptr<IResource> rsrc = deserializer->Deserialize(RESOURCE_KEY);
	auto vecIntResource = static_cast<Resource<VecInt>*>(rsrc.get());
	EXPECT_TRUE(vecIntResource);
	EXPECT_EQ(vecIntResource->Data(), INT_VALUES);

	// clean up
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	ResourceDeserializer::ResetInstance();
	ResourceSerializer::ResetInstance();
}

TEST(ResourceDeserializer, DeserializeThrowsWithoutSerializationPath)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<VecInt>(RESOURCE_KEY);
	EXPECT_THROW(deserializer->Deserialize(RESOURCE_KEY), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, DeserializeThrowsWithBadSerializationPath)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	deserializer->SetSerializationPath(BAD_PATH);
	deserializer->RegisterResource<VecInt>(RESOURCE_KEY);
	EXPECT_THROW(deserializer->Deserialize(RESOURCE_KEY), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

TEST(ResourceDeserializer, DeserializeThrowsWithEmptyKey)
{
	ResourceDeserializer* deserializer = ResourceDeserializer::GetInstance();

	deserializer->SetSerializationPath(RESOURCE_ROOT);
	deserializer->RegisterResource<VecInt>(RESOURCE_KEY);
	EXPECT_THROW(deserializer->Deserialize(""), std::runtime_error);

	ResourceDeserializer::ResetInstance();
}

