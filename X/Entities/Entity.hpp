
template<class T>
void Entity::AggregateMember(std::shared_ptr<T> sharedObj)
{
	if (!sharedObj)
		throw std::runtime_error("Entity is nullptr when aggregating entity");

	Key key = sharedObj->GetKey();

	auto found = membersMap_.find(key);
	if (found != membersMap_.cend())
		throw std::runtime_error("Cannot aggregate entity using key=" + key + " because this key is already in use.");

	membersMap_[key] = std::move(sharedObj);

	RegisterEntityConstructor<T>(key);
}

template<class T>
Entity::SharedEntity& Entity::GetAggregatedMember(const Key& key) const
{
	SharedEntity& obj = GetAggregatedMember(key);
	if (!dynamic_cast<T*>(obj.get()))
		throw std::runtime_error("Key=" + key + " is not an Entity of type=" + typeid(T).name());
	return obj;
}

template<class T>
std::vector<Entity::Key> Entity::GetAggregatedMemberKeys() const
{
	std::vector<Key> keys;
	for (const std::pair<Key, SharedEntity>& e : membersMap_)
	{
		if (dynamic_cast<T*>(e.second.get()))
			keys.push_back(e.first);
	}
	return keys;
}

template<typename T>
void Entity::RegisterEntityConstructor(const Entity::Key& key)
{
	auto TFunction = [](const Entity& other)->std::unique_ptr<T> { return std::make_unique<T>(static_cast<const T&>(other)); };
	keyToEntityConstructorMap_.insert(std::make_pair(key, TFunction));
}
