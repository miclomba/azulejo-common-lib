
template<typename T>
void EntityAggregationDeserializer::RegisterEntity()
{
	auto TFunction = []()->std::unique_ptr<T> { return std::make_unique<T>(); };
	std::string key = TFunction()->GetKey();

	if (keyToEntityMap_.find(key) != keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " is already registered with the EntityAggregationDeserializer");

	keyToEntityMap_.insert(std::make_pair(key, TFunction));
}

template<typename T>
void EntityAggregationDeserializer::UnregisterEntity()
{
	auto tee = std::make_unique<T>();
	std::string key = tee->GetKey();

	if (keyToEntityMap_.find(key) == keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " not already registered with the EntityAggregationDeserializer");

	keyToEntityMap_.erase(key);
}