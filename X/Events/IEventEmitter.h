#ifndef events_ievent_emitter_h
#define events_ievent_emitter_h

#include <memory>
#include <string>

#include "config.h"

#include <boost/signals2/connection.hpp>

namespace events
{

class IEventConsumer;

class EVENTS_DLL_EXPORT IEventEmitter
{
public:
	IEventEmitter();
	virtual ~IEventEmitter();

	IEventEmitter(const IEventEmitter&);
	IEventEmitter& operator=(const IEventEmitter&);
	IEventEmitter(IEventEmitter&&);
	IEventEmitter& operator=(IEventEmitter&&);

	virtual boost::signals2::connection Connect(const std::shared_ptr<IEventConsumer> subscriber) = 0;
	virtual std::string GetSubscriberType() const = 0;
};

} // end namespace events
#endif // events_ievent_emitter_h
