
template <typename T>
void ResourceDeserializer::RegisterResource(const std::string &key, std::function<std::unique_ptr<ISerializableResource>(void)> constructor)
{
	if (!std::is_arithmetic<T>::value)
		throw std::runtime_error("Type (" + std::string(typeid(T).name()) + ") is not an arithmetic container when registering resource with ResourceDeserializer");
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when registering resource with ResourceDeserializer");

	std::lock_guard<std::recursive_mutex> lock(mtx_);

	if (keyToResourceMap_.find(key) != keyToResourceMap_.cend())
		throw std::runtime_error("Key=" + key + " is already registered with the ResourceDeserializer");

	keyToResourceMap_.insert(std::make_pair(key, constructor));
}