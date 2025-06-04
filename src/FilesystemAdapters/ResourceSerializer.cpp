#include "FilesystemAdapters/ResourceSerializer.h"

#include <stdexcept>
#include <string>
#include "Config/filesystem.hpp"

#include <boost/system/error_code.hpp>

#include "FilesystemAdapters/ISerializableResource.h"

using boost::system::error_code;
using filesystem_adapters::ISerializableResource;
using filesystem_adapters::ResourceSerializer;

namespace
{
	const std::string RESOURCE_EXT = ".bin";
}

ResourceSerializer *ResourceSerializer::instance_ = nullptr;

ResourceSerializer::ResourceSerializer() = default;
ResourceSerializer::~ResourceSerializer() = default;

ResourceSerializer *ResourceSerializer::GetInstance()
{
	if (!instance_)
		instance_ = new ResourceSerializer();
	return instance_;
}

void ResourceSerializer::ResetInstance()
{
	if (instance_)
		delete instance_;
	instance_ = nullptr;
}

void ResourceSerializer::SetSerializationPath(const std::string &binaryFilePath)
{
	serializationPath_ = binaryFilePath;
}

std::string ResourceSerializer::GetSerializationPath() const
{
	if (serializationPath_.empty())
		throw std::runtime_error("No serialization path set for the ResourceSerializer");

	return serializationPath_.string();
}

void ResourceSerializer::Serialize(const ISerializableResource &resource, const std::string &key)
{
	if (key.empty())
		throw std::runtime_error("Cannot serialize resource with empty key");

	Path serializationPath = GetSerializationPath();

	if (!fs::exists(serializationPath))
		fs::create_directories(serializationPath);

	if (!resource.UpdateChecksum())
		return;

	const std::string fileName = key + RESOURCE_EXT;
	std::ofstream outfile((serializationPath / fileName).string(), std::ios::binary);
	if (!outfile)
		throw std::runtime_error("Could not open output file: " + (serializationPath / fileName).string());

	const void *data = resource.Data();

	const size_t M = resource.GetColumnSize();
	const char *MBuff = reinterpret_cast<const char *>(&M);
	outfile.write(MBuff, sizeof(size_t));

	const size_t N = resource.GetRowSize();
	const char *NBuff = reinterpret_cast<const char *>(&N);
	outfile.write(NBuff, sizeof(size_t));

	const char *buff = reinterpret_cast<const char *>(data);
	size_t size = resource.GetElementSize() * resource.GetColumnSize() * resource.GetRowSize();
	outfile.write(buff, size);
}

void ResourceSerializer::Unserialize(const std::string &key)
{
	if (key.empty())
		throw std::runtime_error("Cannot unserialize resource with empty key");

	const std::string fileName = key + RESOURCE_EXT;
	Path resourcePath = Path(GetSerializationPath()) / fileName;

	if (fs::exists(resourcePath))
	{
#if defined(__APPLE__) || defined(__MACH__)
		error_code ec;
		fs::permissions(resourcePath, fs::perms::all_all, ec);
		if (ec)
			throw std::runtime_error("ResourceSerializer could not set permissions on file: " + resourcePath.string());
#else
		fs::permissions(resourcePath, fs::perms::all, fs::perm_options::add);
#endif
		fs::remove(resourcePath);
	}
}
