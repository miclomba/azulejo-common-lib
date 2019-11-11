
template<typename T>
void ResourceSerializer::Serialize(const Resource<T>& resource, const std::string& key)
{
	if (key.empty())
		throw std::runtime_error("Cannot serialize resource with empty key");

	const std::string RESOURCE_EXT = ".bin";

	auto serializationPath = std::filesystem::path(GetSerializationPath());

	if (! std::filesystem::exists(serializationPath))
		std::filesystem::create_directories(serializationPath);

	if (!resource.IsDirty())
		return;

	const std::string fileName = key + RESOURCE_EXT;
	std::ofstream outfile(serializationPath / fileName, std::ios::binary);
	if (!outfile)
		throw std::runtime_error("Could not open output file: " + (serializationPath / fileName).string());

	const T* data = resource.Data();

	const size_t M = resource.GetColumnSize();
	const char* MBuff = reinterpret_cast<const char*>(&M);
	outfile.write(MBuff, sizeof(size_t));

	const size_t N = resource.GetRowSize();
	const char* NBuff = reinterpret_cast<const char*>(&N);
	outfile.write(NBuff, sizeof(size_t));

	const char* buff = reinterpret_cast<const char*>(data);
	size_t size = sizeof(T) * resource.GetColumnSize() * resource.GetRowSize();
	outfile.write(buff, size);
}

