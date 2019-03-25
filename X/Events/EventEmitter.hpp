#define TEMPLATE_T template<typename T>

TEMPLATE_T
EventEmitter<T>::EventEmitter()
{
}

TEMPLATE_T
EventEmitter<T>::~EventEmitter()
{
	for (auto& keyNodePair : subscriberMap_)
	{
		boost::signals2::connection& conn = keyNodePair.second;
		conn.disconnect();
	}
}

TEMPLATE_T EventEmitter<T>::EventEmitter(const EventEmitter<T>&) = default;
TEMPLATE_T EventEmitter<T>& EventEmitter<T>::operator=(const EventEmitter<T>&) = default;
TEMPLATE_T EventEmitter<T>::EventEmitter(EventEmitter<T>&&) = default;
TEMPLATE_T EventEmitter<T>& EventEmitter<T>::operator=(EventEmitter<T>&&) = default;

TEMPLATE_T
void EventEmitter<T>::Connect(const std::string& key, const std::function<T>& subscriber)
{
	if (subscriberMap_.find(key) != subscriberMap_.cend())
	{
		subscriberMap_[key].disconnect();
		subscriberMap_.erase(key);
	}

	subscriberMap_[key] = emitter_.connect(subscriber);
}

TEMPLATE_T
void EventEmitter<T>::Disconnect(const std::string& key)
{
	if (subscriberMap_.find(key) != subscriberMap_.cend())
	{
		subscriberMap_[key].disconnect();
		subscriberMap_.erase(key);
	}
	else
	{
		throw std::runtime_error("Cannot remove event subscriber with key=" + key + " from EventEmitter");
	}
}

TEMPLATE_T
std::string EventEmitter<T>::GetSubscriberType() const
{
	return typeid(T).name();
}

TEMPLATE_T
void EventEmitter<T>::Emit() const
{
	emitter_();
}

#undef TEMPLATE_T
