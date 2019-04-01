#include "EventChannel.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "IEventConsumer.h"
#include "IEventEmitter.h"
#include "EventConsumer.h"

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

	if (IsConsumerRegistered(emitterKey))
	{
		//emitterMap_[key]->
	}
}

void EventChannel::UnregisterEmitter(const std::string& emitterKey)
{
	if (!IsEmitterRegistered(emitterKey))
		throw std::runtime_error("IEventEmitter with key=" + emitterKey + " is not registered in this EventChannel");
	emitterMap_.erase(emitterKey);
}

void EventChannel::RegisterConsumer(const std::string& consumerKey, const std::shared_ptr<IEventConsumer> consumer)
{
	if (IsConsumerRegistered(consumerKey))
		throw std::runtime_error("IEventConsumer with key=" + consumerKey + " is already registered in this EventChannel");
	consumerMap_[consumerKey] = consumer;
}

void EventChannel::UnregisterConsumer(const std::string& consumerKey)
{
	if (!IsConsumerRegistered(consumerKey))
		throw std::runtime_error("IEventConsumer with key=" + consumerKey + " is not registered in this EventChannel");
	consumerMap_.erase(consumerKey);
}

bool EventChannel::IsEmitterRegistered(const std::string& emitterKey) const
{
	return emitterMap_.find(emitterKey) != emitterMap_.cend() ? true : false;
}

bool EventChannel::IsConsumerRegistered(const std::string& consumerKey) const
{
	return consumerMap_.find(consumerKey) != consumerMap_.cend() ? true : false;
}

} // end namespace events
