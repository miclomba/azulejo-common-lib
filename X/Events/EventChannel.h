#ifndef events_event_channel_h
#define events_event_channel_h

#include <string>

#include "config.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/slot.hpp>

namespace events
{

class EVENTS_DLL_EXPORT EventChannel
{
public:
	EventChannel();
	virtual ~EventChannel();

	EventChannel(const EventChannel&);
	EventChannel& operator=(const EventChannel&);
	EventChannel(EventChannel&&);
	EventChannel& operator=(EventChannel&&);

private:
	std::vector<boost::signals2::connection> connections_;
};

//#include "EventChannel.hpp"

} // end namespace events
#endif // events_event_channel_h
