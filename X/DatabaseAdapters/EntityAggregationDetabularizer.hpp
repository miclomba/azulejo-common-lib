
template<typename T>
void EntityAggregationDetabularizer::RegisterEntity(const std::string& key)
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when registering entity with EntityAggregationDetabularizer");

	if (keyToEntityMap_.find(key) != keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " is already registered with the EntityAggregationDetabularizer");

	auto TFunction = []()->std::unique_ptr<T> { return std::make_unique<T>(); };
	keyToEntityMap_.insert(std::make_pair(key, TFunction));
}
