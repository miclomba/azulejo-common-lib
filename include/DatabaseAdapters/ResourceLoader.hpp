
template<typename T>
void ResourceLoader::RegisterResource(const std::string_view key, std::function<std::unique_ptr<IPersistableResource>(void)> constructor)
{
	if (!std::is_arithmetic<T>::value)
		throw std::runtime_error("Type (" + std::string(typeid(T).name()) + ") is not an arithmetic container when registering resource with ResourceDeserializer");
	if (key.empty())
		throw std::runtime_error("Key is empty when registering resource with ResourceDeserializer");
	if (keyToResourceMap_.find(key) != keyToResourceMap_.cend())
		throw std::runtime_error("Key is already registered with the ResourceDeserializer");

	keyToResourceMap_.insert(std::make_pair(key, constructor));
}