#ifndef events_event_emitter_h
#define events_event_emitter_h

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>

#include "config.h"

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
	EventEmitter();
	virtual ~EventEmitter();

	EventEmitter(const EventEmitter&);
	EventEmitter& operator=(const EventEmitter&);
	EventEmitter(EventEmitter&&);
	EventEmitter& operator=(EventEmitter&&);

	void Connect(const std::string& key, const std::shared_ptr<IEventConsumer> subscriber) override;
	void Emit() const override;
	std::string GetSubscriberType() const override;

private:
	void Connect(const std::string& key, const std::function<T>& subscriber);

	boost::signals2::signal<T> emitter_;
};

#include "EventEmitter.hpp"
} // end namespace events
#endif // events_event_emitter_h
