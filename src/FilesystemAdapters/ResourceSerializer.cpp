#include "FilesystemAdapters/ResourceSerializer.h"

#include <mutex>
#include <stdexcept>
#include <string>
#include "Config/filesystem.hpp"

#include <boost/system/error_code.hpp>

#include "FilesystemAdapters/FileLock.hpp"
#include "FilesystemAdapters/ISerializableResource.h"

using boost::system::error_code;
using filesystem_adapters::GetGlobalFileLock;
using filesystem_adapters::ISerializableResource;
using filesystem_adapters::ResourceSerializer;

using LockedResource = filesystem_adapters::ISerializableResource::LockedResource;

namespace
{
	const std::string RESOURCE_EXT = ".bin";
	const std::string RESOURCE_TMP_EXT = ".tmp";
}

ResourceSerializer::ResourceSerializer() = default;
ResourceSerializer::~ResourceSerializer() = default;

ResourceSerializer *ResourceSerializer::GetInstance()
{
	static ResourceSerializer instance;
	return &instance;
}

void ResourceSerializer::Serialize(const LockedResource &resource, const std::string &key, const std::string &serializationPath)
{
	if (key.empty())
		throw std::runtime_error("Cannot serialize resource with empty key");

	Path resourcePath = serializationPath;

	std::lock_guard<std::recursive_mutex> lock(GetGlobalFileLock());

	error_code ec;
	fs::create_directories(resourcePath, ec);
	if (ec)
		throw std::runtime_error("ResourceSerializer could not create the directory: " + resourcePath.string());

	if (!resource.UpdateChecksum())
		return;

	const std::string fileName = key + RESOURCE_EXT;
	const std::string tmpFileName = key + RESOURCE_TMP_EXT;
	std::ofstream outfile((resourcePath / tmpFileName).string(), std::ios::binary);
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

	// move temp file to actual file
	outfile.close();
	fs::rename(resourcePath / tmpFileName, resourcePath / fileName, ec);
	if (ec)
		throw std::runtime_error("ResourceSerializer could .tmp file to file: " + fileName);
}

void ResourceSerializer::Unserialize(const std::string &key, const std::string &serializationPath)
{
	if (key.empty())
		throw std::runtime_error("Cannot unserialize resource with empty key");

	const std::string fileName = key + RESOURCE_EXT;
	Path resourcePath = Path(serializationPath) / fileName;

	std::lock_guard<std::recursive_mutex> lock(GetGlobalFileLock());

	if (fs::exists(resourcePath))
	{
		error_code ec;
#if defined(__APPLE__) || defined(__MACH__)
		fs::permissions(resourcePath, fs::perms::all_all, ec);
#else
		fs::permissions(resourcePath, fs::perms::all, fs::perm_options::add, ec);
#endif
		if (ec)
			throw std::runtime_error("ResourceSerializer could not set permissions on file: " + resourcePath.string());
		fs::remove(resourcePath, ec);
		if (ec)
			throw std::runtime_error("ResourceSerializer could not remove file: " + resourcePath.string());
	}
}
