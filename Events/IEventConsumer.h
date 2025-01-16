#ifndef events_ievent_consumer_h
#define events_ievent_consumer_h

#include <string>

#include "config.h"

namespace events
{

class EVENTS_DLL_EXPORT IEventConsumer
{
public:
	IEventConsumer();
	virtual ~IEventConsumer();

	IEventConsumer(const IEventConsumer&);
	IEventConsumer& operator=(const IEventConsumer&);
	IEventConsumer(IEventConsumer&&);
	IEventConsumer& operator=(IEventConsumer&&);

	virtual std::string GetSubscriberType() const = 0;
};

} // end namespace events
#endif // events_ievent_consumer_h
