#ifndef events_event_emitter_h
#define events_event_emitter_h

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include "config.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>

#include "IEventConsumer.h"
#include "IEventEmitter.h"
#include "EventConsumer.h"

namespace events
{

template<typename T>
class EVENTS_DLL_EXPORT EventEmitter : public IEventEmitter
{
public:
	typedef boost::signals2::signal<T> signal_t;

public:
	EventEmitter();
	virtual ~EventEmitter();

	EventEmitter(const EventEmitter&);
	EventEmitter& operator=(const EventEmitter&);
	EventEmitter(EventEmitter&&);
	EventEmitter& operator=(EventEmitter&&);

	boost::signals2::connection Connect(const std::shared_ptr<IEventConsumer> subscriber) override;
	std::string GetSubscriberType() const override;

	const boost::signals2::signal<T>& Signal() const;
private:
	boost::signals2::signal<T> emitter_;
};

#include "EventEmitter.hpp"
} // end namespace events
#endif // events_event_emitter_h
