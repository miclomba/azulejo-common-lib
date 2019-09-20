#define TEMPLATE_T template<typename T>

TEMPLATE_T
EventEmitter<T>::EventEmitter()
{
}

TEMPLATE_T EventEmitter<T>::~EventEmitter() = default;
TEMPLATE_T EventEmitter<T>::EventEmitter(EventEmitter<T>&&) = default;
TEMPLATE_T EventEmitter<T>& EventEmitter<T>::operator=(EventEmitter<T>&&) = default;

TEMPLATE_T
boost::signals2::connection EventEmitter<T>::Connect(const std::shared_ptr<IEventConsumer> subscriber)
{
	auto slot = std::dynamic_pointer_cast<EventConsumer<T>>(subscriber);
	if (!slot)
		throw std::runtime_error("Cannot connect event emitter to event consumer because event consumer is not of type=" + GetSubscriberType());

	return emitter_.connect(EventEmitter<T>::signal_t::slot_type(slot->GetSubscriber()).track_foreign(slot));
}

TEMPLATE_T
std::string EventEmitter<T>::GetSubscriberType() const
{
	return typeid(T).name();
}

TEMPLATE_T
const boost::signals2::signal<T>& EventEmitter<T>::Signal() const
{
	return emitter_;
}

#undef TEMPLATE_T
