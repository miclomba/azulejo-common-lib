
template<typename T>
void ResourceDeserializer::RegisterResource(const std::string& key)
{
	if (!is_arithmetic_container<T>::value)
		throw std::runtime_error("Type (" + std::string(typeid(T).name()) + ") is not an arithmetic container when registering resource with ResourceDeserializer");
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when registering resource with ResourceDeserializer");

	auto TFunction = []()->std::unique_ptr<IResource> { return std::make_unique<Resource<T>>(); };

	if (keyToResourceMap_.find(key) != keyToResourceMap_.cend())
		throw std::runtime_error("Key=" + key + " is already registered with the ResourceDeserializer");

	keyToResourceMap_.insert(std::make_pair(key, TFunction));
}