
#define TEMPLATE_T template<typename EntityStorageType>
#define EntityRegistry_t EntityRegistry<EntityStorageType>

TEMPLATE_T
EntityRegistry_t::EntityRegistry() = default;

TEMPLATE_T
EntityRegistry_t::EntityRegistry(const EntityRegistry_t&) = default;

TEMPLATE_T
EntityRegistry_t& EntityRegistry_t::operator=(const EntityRegistry_t&) = default;

TEMPLATE_T
EntityRegistry_t::EntityRegistry(EntityRegistry_t&&) = default;

TEMPLATE_T
EntityRegistry_t& EntityRegistry_t::operator=(EntityRegistry_t&&) = default;

TEMPLATE_T
EntityRegistry_t::~EntityRegistry() = default;

TEMPLATE_T
bool EntityRegistry_t::HasRegisteredKey(const Entity::Key& key) const
{
	return keyToEntityMap_.find(key) != keyToEntityMap_.cend();
}

TEMPLATE_T
void EntityRegistry_t::UnregisterEntity(const Entity::Key& key)
{
	if (keyToEntityMap_.find(key) == keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " not already registered with the EntityRegistry");

	keyToEntityMap_.erase(key);
}

TEMPLATE_T
void EntityRegistry_t::UnregisterAll()
{
	keyToEntityMap_.clear();
}

TEMPLATE_T
std::unique_ptr<EntityStorageType> EntityRegistry_t::GenerateEntity(const Entity::Key& key) const
{
	if (!HasRegisteredKey(key))
		throw std::runtime_error("Key=" + key + " is not registered with the EntityRegistry");

	std::unique_ptr<EntityStorageType> entity = keyToEntityMap_[key]();
	entity->SetKey(key);

	return std::move(entity);
}

TEMPLATE_T
template<typename T>
void EntityRegistry_t::RegisterEntity(const std::string& key)
{
	if (key.empty())
		throw std::runtime_error("Key (" + key + ") is empty when registering entity with EntityRegistry");

	if (keyToEntityMap_.find(key) != keyToEntityMap_.cend())
		throw std::runtime_error("Key=" + key + " is already registered with the EntityRegistry");

	auto TFunction = []()->std::unique_ptr<T> { return std::make_unique<T>(); };
	keyToEntityMap_.insert(std::make_pair(key, TFunction));
}

#undef TEMPLATE_T
#undef EntityRegistry_t
