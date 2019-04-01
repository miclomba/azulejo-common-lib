#ifndef events_ievent_emitter_h
#define events_ievent_emitter_h

#include <map>
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

	void Disconnect(const std::string& key);

	virtual void Connect(const std::string& key, const std::shared_ptr<IEventConsumer> subscriber) = 0;
	virtual void Emit() const = 0;
	virtual std::string GetSubscriberType() const = 0;

protected:
	std::map<std::string, boost::signals2::connection> subscriberMap_;

};

} // end namespace events
#endif // events_ievent_emitter_h
