
template<typename T>
void ResourceSerializer::Serialize(const std::string& key, const IResource<T>& resource)
{
	// validate
	if (serializationPath_.empty())
		throw std::runtime_error("No serialization path set for the ResourceSerializer");
	if (!serializationStructure_)
		throw std::runtime_error("No serialization structure set for the ResourceSerializer");

	if (resource.Data().empty() || !resource.IsDirty())
		return;

	// setup
	auto& data = resource.Data();
	const char* buff = reinterpret_cast<const char*>(data.data());
	int size = sizeof(data[0]) * data.size();
	const std::string RESOURCE_KEY = "resource";
	const std::string RESOURCE_EXT = ".bin";

	// serialize to serialization structure
	serializationStructure_->put(RESOURCE_KEY, key + ":" + std::to_string(size));

	// serialize to serialization path
	if (! fs::exists(serializationPath_))
		fs::create_directories(serializationPath_);

	const std::string fileName = key + RESOURCE_EXT;
	std::ofstream outfile(serializationPath_ / fileName, std::ios::binary);
	if (!outfile)
		throw std::runtime_error("Could not open output file: " + serializationPath_.string());

	outfile.write(buff, sizeof(data[0]) * data.size());
}

