#include "EventChannel.h"

namespace events
{

EventChannel::EventChannel()
{
}

EventChannel::~EventChannel()
{
	for (boost::signals2::connection& conn : connections_)
		conn.disconnect();
}

EventChannel::EventChannel(const EventChannel&) = default;
EventChannel& EventChannel::operator=(const EventChannel&) = default;
EventChannel::EventChannel(EventChannel&&) = default;
EventChannel& EventChannel::operator=(EventChannel&&) = default;

}
