#define TEMPLATE_T template<typename T>

TEMPLATE_T
EventEmitter<T>::EventEmitter()
{
}

TEMPLATE_T
EventEmitter<T>::~EventEmitter()
{
	for (boost::signals2::connection& conn : connections_)
		conn.disconnect();
}

TEMPLATE_T EventEmitter<T>::EventEmitter(const EventEmitter<T>&) = default;
TEMPLATE_T EventEmitter<T>& EventEmitter<T>::operator=(const EventEmitter<T>&) = default;
TEMPLATE_T EventEmitter<T>::EventEmitter(EventEmitter<T>&&) = default;
TEMPLATE_T EventEmitter<T>& EventEmitter<T>::operator=(EventEmitter<T>&&) = default;

TEMPLATE_T
void EventEmitter<T>::Connect(const typename EventEmitter<T>::emitter_signal_t::slot_type& subscriber)
{
	connections_.push_back(emitter_.connect(subscriber));
}

#undef TEMPLATE_T
