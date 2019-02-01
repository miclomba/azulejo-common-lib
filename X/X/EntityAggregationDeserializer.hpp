
template<typename T>
void EntityAggregationDeserializer::RegisterEntity(const std::string& key)
{
	auto TFunction = []()->std::unique_ptr<T> { return std::make_unique<T>(); };

	if (keyToEntityMap_.find(key) != keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " is already registered with the EntityAggregationDeserializer");

	keyToEntityMap_.insert(std::make_pair(key, TFunction));
}
