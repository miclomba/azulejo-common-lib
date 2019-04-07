#define TEMPLATE_T template<typename T>

TEMPLATE_T
EventConsumer<T>::EventConsumer(const std::function<T>& subscriber) :
	subscriber_(subscriber)
{
}

TEMPLATE_T EventConsumer<T>::~EventConsumer() = default;
TEMPLATE_T EventConsumer<T>::EventConsumer(const EventConsumer<T>&) = default;
TEMPLATE_T EventConsumer<T>& EventConsumer<T>::operator=(const EventConsumer<T>&) = default;
TEMPLATE_T EventConsumer<T>::EventConsumer(EventConsumer<T>&&) = default;
TEMPLATE_T EventConsumer<T>& EventConsumer<T>::operator=(EventConsumer<T>&&) = default;

TEMPLATE_T 
std::function<T> EventConsumer<T>::GetSubscriber() const
{
	return subscriber_;
}

TEMPLATE_T
std::string EventConsumer<T>::GetSubscriberType() const
{
	return typeid(T).name();
}

#undef TEMPLATE_T
