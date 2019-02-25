
template<typename T>
void ResourceSerializer::Serialize(const Resource<T>& resource, const std::string& key)
{
	const std::string RESOURCE_EXT = ".bin";

	auto serializationPath = fs::path(GetSerializationPath());

	if (! fs::exists(serializationPath))
		fs::create_directories(serializationPath);

	if (resource.IsDirty())
	{
		auto& data = resource.Data();
		const char* buff = reinterpret_cast<const char*>(data.data());
		int size = sizeof(data[0]) * data.size();

		const std::string fileName = key + RESOURCE_EXT;
		std::ofstream outfile(serializationPath / fileName, std::ios::binary);
		if (!outfile)
			throw std::runtime_error("Could not open output file: " + (serializationPath / fileName).string());

		outfile.write(buff, sizeof(data[0]) * data.size());
	}
}

