#ifndef events_event_channel_h
#define events_event_channel_h

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "config.h"

namespace events
{

class IEventEmitter;
class IEventConsumer;

class EVENTS_DLL_EXPORT EventChannel
{
public:
	EventChannel();
	virtual ~EventChannel();

	EventChannel(const EventChannel&);
	EventChannel& operator=(const EventChannel&);
	EventChannel(EventChannel&&);
	EventChannel& operator=(EventChannel&&);

	void RegisterEmitter(const std::string& emitterKey, const std::shared_ptr<IEventEmitter> emitter);
	void UnregisterEmitter(const std::string& emitterKey);

	void RegisterConsumer(const std::string& consumerKey, const std::string& emitterKey, const std::shared_ptr<IEventConsumer> consumer);
	void UnregisterConsumer(const std::string& consumerKey, const std::string& emitterKey);

private:
	bool IsEmitterRegistered(const std::string& emitterKey) const;
	bool IsConsumerRegistered(const std::string& consumerKey, const std::string& emitterKey) const;

	std::map<std::string, std::shared_ptr<IEventEmitter>> emitterMap_;
	std::map<std::string, std::shared_ptr<IEventConsumer>> consumerMap_;
};

} // end namespace events
#endif // events_event_channel_h
