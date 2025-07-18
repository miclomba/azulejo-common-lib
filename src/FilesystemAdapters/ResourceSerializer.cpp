#include "FilesystemAdapters/ResourceSerializer.h"

#include <mutex>
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

ResourceSerializer::ResourceSerializer() = default;
ResourceSerializer::~ResourceSerializer() = default;

ResourceSerializer *ResourceSerializer::GetInstance()
{
	static ResourceSerializer instance;
	return &instance;
}

void ResourceSerializer::Serialize(const ISerializableResource &resource, const std::string &key, const std::string &serializationPath)
{
	if (key.empty())
		throw std::runtime_error("Cannot serialize resource with empty key");

	Path resourcePath = serializationPath;

	std::lock_guard<std::mutex> lock(mtx_);

	if (!fs::exists(resourcePath))
		fs::create_directories(resourcePath);

	if (!resource.UpdateChecksum())
		return;

	const std::string fileName = key + RESOURCE_EXT;
	std::ofstream outfile((resourcePath / fileName).string(), std::ios::binary);
	if (!outfile)
		throw std::runtime_error("Could not open output file: " + (resourcePath / fileName).string());

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

void ResourceSerializer::Unserialize(const std::string &key, const std::string &serializationPath)
{
	if (key.empty())
		throw std::runtime_error("Cannot unserialize resource with empty key");

	const std::string fileName = key + RESOURCE_EXT;
	Path resourcePath = Path(serializationPath) / fileName;

	std::lock_guard<std::mutex> lock(mtx_);

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
