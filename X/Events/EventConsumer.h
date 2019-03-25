#ifndef events_event_consumer_h
#define events_event_consumer_h

#include <functional>

#include "config.h"

namespace events
{

template<typename T>
class EVENTS_DLL_EXPORT EventConsumer
{
public:
	EventConsumer(const std::function<T>& subscriber);
	virtual ~EventConsumer();

	EventConsumer(const EventConsumer&);
	EventConsumer& operator=(const EventConsumer&);
	EventConsumer(EventConsumer&&);
	EventConsumer& operator=(EventConsumer&&);

	std::function<T> GetSubscriber() const;

private:
	std::function<T> subscriber_;
};

#include "EventConsumer.hpp"

} // end namespace events
#endif // events_event_consumer_h
