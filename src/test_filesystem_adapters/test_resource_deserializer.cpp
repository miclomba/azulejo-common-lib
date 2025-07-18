#include "test_filesystem_adapters/config.h"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "Config/filesystem.hpp"

#include <gtest/gtest.h>

#include "test_filesystem_adapters/ContainerResource.h"
#include "test_filesystem_adapters/ContainerResource2D.h"
#include "FilesystemAdapters/ISerializableResource.h"
#include "FilesystemAdapters/ResourceDeserializer.h"
#include "FilesystemAdapters/ResourceSerializer.h"

using filesystem_adapters::ISerializableResource;
using filesystem_adapters::ResourceDeserializer;
using filesystem_adapters::ResourceSerializer;

using Resource = ContainerResource<int>;
using Resource2D = ContainerResource2D<int>;

namespace
{
	const std::string RESOURCE_ROOT = (fs::path(ROOT_FILESYSTEM) / TEST_DIRECTORY).string();
	const std::string RESOURCE_KEY = "resource";
	const std::string BAD_PATH = "$helloworld$";
	const fs::path RESOURCE_FILE = fs::path(RESOURCE_ROOT) / (RESOURCE_KEY + ".bin");
	const std::vector<std::vector<int>> INT_VALUES(1, std::vector<int>(1, 1));
	const std::vector<int> INT_VALUES_ARRAY(1, 1);

	auto RESOURCE_CONSTRUCTOR = []() -> std::unique_ptr<ISerializableResource>
	{ return std::make_unique<Resource>(); };
	auto RESOURCE_2D_CONSTRUCTOR = []() -> std::unique_ptr<ISerializableResource>
	{ return std::make_unique<Resource2D>(); };
} // end namespace

TEST(ResourceDeserializer, GetInstance)
{
	EXPECT_NO_THROW(ResourceDeserializer::GetInstance());
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();
	EXPECT_TRUE(deserializer);
	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, RegisterResource)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, RegisterResourceThrowsOnEmptyKey)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->RegisterResource<int>("", RESOURCE_CONSTRUCTOR), std::runtime_error);

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, RegisterResourceThrowsOnExistingKey)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR), std::runtime_error);

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, UnregisterResource)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_NO_THROW(deserializer->UnregisterResource(RESOURCE_KEY));

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, UnregisterResourceThrowsOnEmptyKey)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->UnregisterResource(""), std::runtime_error);

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, UnregisterResourceThrowsOnUnregisteredKey)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->UnregisterResource(RESOURCE_KEY), std::runtime_error);

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, UnregisterAll)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(RESOURCE_KEY));
	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_TRUE(deserializer->HasSerializationKey(RESOURCE_KEY));
	EXPECT_NO_THROW(deserializer->UnregisterAll());
	EXPECT_FALSE(deserializer->HasSerializationKey(RESOURCE_KEY));

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, HasSerializationKeyFalse)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_FALSE(deserializer->HasSerializationKey(RESOURCE_KEY));

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, HasSerializationKeyTrue)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_NO_THROW(deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR));
	EXPECT_TRUE(deserializer->HasSerializationKey(RESOURCE_KEY));

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, GenerateResource)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	std::unique_ptr<ISerializableResource> resource = deserializer->GenerateResource(RESOURCE_KEY);
	EXPECT_TRUE(resource);

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, GenerateResourceThrowsOnEmptyKey)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->GenerateResource(""), std::runtime_error);

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, GenerateResourceThrowsOnUnregisteredKey)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	EXPECT_THROW(deserializer->GenerateResource(RESOURCE_KEY), std::runtime_error);

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, DeserializeArray)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	// serialize
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
	Resource resource(INT_VALUES_ARRAY);
	serializer->Serialize(resource, RESOURCE_KEY, RESOURCE_ROOT);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));

	// deserialize
	deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	std::unique_ptr<ISerializableResource> rsrc = deserializer->Deserialize(RESOURCE_KEY, RESOURCE_ROOT);
	auto vecIntResource = static_cast<Resource *>(rsrc.get());
	EXPECT_TRUE(vecIntResource);
	EXPECT_EQ(*static_cast<int *>(vecIntResource->Data()), *static_cast<int *>(resource.Data()));
	EXPECT_EQ(vecIntResource->GetColumnSize(), resource.GetColumnSize());
	EXPECT_EQ(vecIntResource->GetRowSize(), resource.GetRowSize());

	// clean up
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, DeserializeMatrix)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	// serialize
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
	Resource2D resource(INT_VALUES);
	serializer->Serialize(resource, RESOURCE_KEY, RESOURCE_ROOT);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));

	// deserialize
	deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_2D_CONSTRUCTOR);
	std::unique_ptr<ISerializableResource> rsrc = deserializer->Deserialize(RESOURCE_KEY, RESOURCE_ROOT);
	auto vecIntResource = static_cast<Resource2D *>(rsrc.get());
	EXPECT_TRUE(vecIntResource);
	EXPECT_EQ(*static_cast<int *>(vecIntResource->Data()), *static_cast<int *>(resource.Data()));
	EXPECT_EQ(vecIntResource->GetColumnSize(), resource.GetColumnSize());
	EXPECT_EQ(vecIntResource->GetRowSize(), resource.GetRowSize());

	// clean up
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, DeserializeEmptyArrayResource)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	// serialize
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
	Resource resource;
	serializer->Serialize(resource, RESOURCE_KEY, RESOURCE_ROOT);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));

	// deserialize
	deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	std::unique_ptr<ISerializableResource> rsrc = deserializer->Deserialize(RESOURCE_KEY, RESOURCE_ROOT);
	auto vecIntResource = static_cast<Resource *>(rsrc.get());
	EXPECT_TRUE(vecIntResource);
	EXPECT_EQ(vecIntResource->Data(), resource.Data());
	EXPECT_EQ(vecIntResource->Data(), static_cast<int *>(nullptr));
	EXPECT_EQ(vecIntResource->GetColumnSize(), resource.GetColumnSize());
	EXPECT_EQ(vecIntResource->GetRowSize(), resource.GetRowSize());

	// clean up
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, DeserializeEmptyMatrixResource)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();
	ResourceSerializer *serializer = ResourceSerializer::GetInstance();

	// serialize
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));
	Resource2D resource;
	serializer->Serialize(resource, RESOURCE_KEY, RESOURCE_ROOT);
	EXPECT_TRUE(fs::exists(RESOURCE_FILE));

	// deserialize
	deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_2D_CONSTRUCTOR);
	std::unique_ptr<ISerializableResource> rsrc = deserializer->Deserialize(RESOURCE_KEY, RESOURCE_ROOT);
	auto vecIntResource = static_cast<Resource2D *>(rsrc.get());
	EXPECT_TRUE(vecIntResource);
	EXPECT_EQ(vecIntResource->Data(), resource.Data());
	EXPECT_EQ(vecIntResource->Data(), static_cast<int *>(nullptr));
	EXPECT_EQ(vecIntResource->GetColumnSize(), resource.GetColumnSize());
	EXPECT_EQ(vecIntResource->GetRowSize(), resource.GetRowSize());

	// clean up
	fs::remove(RESOURCE_FILE);
	EXPECT_FALSE(fs::exists(RESOURCE_FILE));

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, DeserializeThrowsWithoutSerializationPath)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	EXPECT_THROW(deserializer->Deserialize(RESOURCE_KEY, std::string()), std::runtime_error);

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, DeserializeThrowsWithBadSerializationPath)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	EXPECT_THROW(deserializer->Deserialize(RESOURCE_KEY, BAD_PATH), std::runtime_error);

	deserializer->UnregisterAll();
}

TEST(ResourceDeserializer, DeserializeThrowsWithEmptyKey)
{
	ResourceDeserializer *deserializer = ResourceDeserializer::GetInstance();

	deserializer->RegisterResource<int>(RESOURCE_KEY, RESOURCE_CONSTRUCTOR);
	EXPECT_THROW(deserializer->Deserialize("", RESOURCE_ROOT), std::runtime_error);

	deserializer->UnregisterAll();
}
