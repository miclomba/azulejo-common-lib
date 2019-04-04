#include "EventChannel.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "IEventConsumer.h"
#include "IEventEmitter.h"
#include "EventConsumer.h"

namespace
{
std::string Concat(const std::string& consumerKey, const std::string& emitterKey)
{
	return consumerKey + "->" + emitterKey;
}
}

namespace events
{

EventChannel::EventChannel() = default;
EventChannel::~EventChannel() = default;
EventChannel::EventChannel(const EventChannel&) = default;
EventChannel& EventChannel::operator=(const EventChannel&) = default;
EventChannel::EventChannel(EventChannel&&) = default;
EventChannel& EventChannel::operator=(EventChannel&&) = default;

void EventChannel::RegisterEmitter(const std::string& emitterKey, const std::shared_ptr<IEventEmitter> emitter)
{
	if (IsEmitterRegistered(emitterKey))
		throw std::runtime_error("IEventEmitter with key=" + emitterKey + " is already registered in this EventChannel");

	emitterMap_[emitterKey] = emitter;

	//if (IsConsumerRegistered(emitterKey))
	//{
		//emitterMap_[key]->
	//}
}

void EventChannel::UnregisterEmitter(const std::string& emitterKey)
{
	if (!IsEmitterRegistered(emitterKey))
		throw std::runtime_error("IEventEmitter with key=" + emitterKey + " is not registered in this EventChannel");
	emitterMap_.erase(emitterKey);
}

bool EventChannel::IsEmitterRegistered(const std::string& emitterKey) const
{
	return emitterMap_.find(emitterKey) != emitterMap_.cend() ? true : false;
}

void EventChannel::RegisterConsumer(const std::string& consumerKey, const std::string& emitterKey, const std::shared_ptr<IEventConsumer> consumer)
{
	std::string key = Concat(consumerKey, emitterKey);
	if (IsConsumerRegistered(consumerKey, emitterKey))
		throw std::runtime_error("IEventConsumer with key=" + key + " is already registered in this EventChannel");
	consumerMap_[key] = consumer;
}

void EventChannel::UnregisterConsumer(const std::string& consumerKey, const std::string& emitterKey)
{
	std::string key = Concat(consumerKey, emitterKey);
	if (!IsConsumerRegistered(consumerKey, emitterKey))
		throw std::runtime_error("IEventConsumer with key=" + key + " is not registered in this EventChannel");
	consumerMap_.erase(key);
}

bool EventChannel::IsConsumerRegistered(const std::string& consumerKey, const std::string& emitterKey) const
{
	std::string key = Concat(consumerKey, emitterKey);
	return consumerMap_.find(key) != consumerMap_.cend() ? true : false;
}

} // end namespace events
