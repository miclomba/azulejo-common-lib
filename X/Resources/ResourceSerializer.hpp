
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

	const std::vector<std::vector<T>>& data = resource.Data();
	for (const std::vector<int>& row : data)
	{
		const char* buff = reinterpret_cast<const char*>(row.data());
		int size = sizeof(T) * row.size();

		const std::string fileName = key + RESOURCE_EXT;
		std::ofstream outfile(serializationPath / fileName, std::ios::binary);
		if (!outfile)
			throw std::runtime_error("Could not open output file: " + (serializationPath / fileName).string());

		outfile.write(buff, size);
	}
}

