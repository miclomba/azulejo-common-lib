#define TEMPLATE_T template<typename T>

TEMPLATE_T
EventEmitter<T>::EventEmitter()
{
}

TEMPLATE_T EventEmitter<T>::~EventEmitter() = default;
TEMPLATE_T EventEmitter<T>::EventEmitter(const EventEmitter<T>&) = default;
TEMPLATE_T EventEmitter<T>& EventEmitter<T>::operator=(const EventEmitter<T>&) = default;
TEMPLATE_T EventEmitter<T>::EventEmitter(EventEmitter<T>&&) = default;
TEMPLATE_T EventEmitter<T>& EventEmitter<T>::operator=(EventEmitter<T>&&) = default;

TEMPLATE_T
void EventEmitter<T>::Connect(const std::string& key, const std::shared_ptr<IEventConsumer> subscriber)
{
	auto slot = std::dynamic_pointer_cast<EventConsumer<T>>(subscriber);
	if (!slot)
		throw std::runtime_error("Cannot connect event emitter to event consumer because event consumer is not of type=" + GetSubscriberType());

	Connect(key, slot->GetSubscriber());
}

TEMPLATE_T
void EventEmitter<T>::Emit() const
{
	emitter_();
}


TEMPLATE_T
std::string EventEmitter<T>::GetSubscriberType() const
{
	return typeid(T).name();
}


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

#undef TEMPLATE_T
