#ifndef events_event_emitter_h
#define events_event_emitter_h

#include <string>
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/slot.hpp>

#include "config.h"

namespace events
{

template<typename T>
class EVENTS_DLL_EXPORT EventEmitter
{
public:
	typedef boost::signals2::signal<T> emitter_signal_t;

	EventEmitter();
	virtual ~EventEmitter();

	EventEmitter(const EventEmitter&);
	EventEmitter& operator=(const EventEmitter&);
	EventEmitter(EventEmitter&&);
	EventEmitter& operator=(EventEmitter&&);

	void Connect(const typename emitter_signal_t::slot_type& subscriber);

private:
	emitter_signal_t emitter_;
	std::vector<boost::signals2::connection> connections_;
};

#include "EventEmitter.hpp"
} // end namespace events
#endif // events_event_emitter_h
