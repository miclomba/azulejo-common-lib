#ifndef events_event_emitter_h
#define events_event_emitter_h

#include <functional>
#include <map>
#include <string>
#include <typeinfo>

#include "config.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>

namespace events
{

template<typename T>
class EVENTS_DLL_EXPORT EventEmitter
{
public:
	EventEmitter();
	virtual ~EventEmitter();

	EventEmitter(const EventEmitter&);
	EventEmitter& operator=(const EventEmitter&);
	EventEmitter(EventEmitter&&);
	EventEmitter& operator=(EventEmitter&&);

	void Connect(const std::string& key, const std::function<T>& subscriber);
	void Disconnect(const std::string& key);

	std::string GetSubscriberType() const;

	void Emit() const;

private:
	boost::signals2::signal<T> emitter_;

	std::map<std::string, boost::signals2::connection> subscriberMap_;
};

#include "EventEmitter.hpp"
} // end namespace events
#endif // events_event_emitter_h
