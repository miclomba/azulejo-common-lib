
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