#include "EventChannel.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "IEventConsumer.h"
#include "IEventEmitter.h"
#include "EventConsumer.h"

using events::EventChannel;
using events::IEventConsumer;
using events::IEventEmitter;

EventChannel::EventChannel() = default;
EventChannel::~EventChannel() = default;
EventChannel::EventChannel(const EventChannel&) = default;
EventChannel& EventChannel::operator=(const EventChannel&) = default;
EventChannel::EventChannel(EventChannel&&) = default;
EventChannel& EventChannel::operator=(EventChannel&&) = default;

void EventChannel::RegisterEmitter(const std::string& emitterKey, const std::shared_ptr<IEventEmitter> emitter)
{
	if (emitterKey.empty())
		throw std::invalid_argument("Cannot register emitter with empty key in EventChannel.");
	if (!emitter)
		throw std::invalid_argument("Cannot register NULL emitter in EventChannel.");

	if (IsEmitterRegistered(emitterKey))
		throw std::runtime_error("IEventEmitter with key=" + emitterKey + " is already registered in this EventChannel");

	for (auto iter = consumerMap_.begin(); iter != consumerMap_.end(); ++iter)
	{
		const std::pair<std::string, std::string> consumerKey = iter->first;
		std::weak_ptr<IEventConsumer> consumer = iter->second;
		if (consumerKey.second == emitterKey && !consumer.expired())
			emitter->Connect(consumer.lock());
	}

	emitterMap_[emitterKey] = emitter;
}

void EventChannel::UnregisterEmitter(const std::string& emitterKey)
{
	if (emitterKey.empty())
		throw std::invalid_argument("Cannot unregister emitter with empty key in EventChannel.");

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
	if (consumerKey.empty() || emitterKey.empty())
		throw std::invalid_argument("Cannot register consumer with empty key in EventChannel.");
	if (!consumer)
		throw std::invalid_argument("Cannot register NULL consumer in EventChannel.");

	std::pair<std::string, std::string> key = std::make_pair(consumerKey, emitterKey);
	if (IsConsumerRegistered(consumerKey, emitterKey))
		throw std::runtime_error("IEventConsumer with key=[" + key.first + "," + key.second + "] is already registered in this EventChannel");

	for (auto iter = emitterMap_.begin(); iter != emitterMap_.end(); ++iter)
	{
		const std::string& key = iter->first;
		std::weak_ptr<IEventEmitter> emitter = iter->second;
		if (key == emitterKey && !emitter.expired())
		{
			std::shared_ptr<IEventEmitter> eventEmitter = emitter.lock();
			eventEmitter->Connect(consumer);
		}
	}

	consumerMap_[key] = consumer;
}

void EventChannel::UnregisterConsumer(const std::string& consumerKey, const std::string& emitterKey)
{
	if (consumerKey.empty() || emitterKey.empty())
		throw std::invalid_argument("Cannot unregister consumer with empty key in EventChannel.");

	std::pair<std::string, std::string> key = std::make_pair(consumerKey, emitterKey);
	if (!IsConsumerRegistered(consumerKey, emitterKey))
		throw std::runtime_error("IEventConsumer with key=[" + key.first + "," + key.second + "] is not registered in this EventChannel");
	consumerMap_.erase(key);
}

bool EventChannel::IsConsumerRegistered(const std::string& consumerKey, const std::string& emitterKey) const
{
	std::pair<std::string, std::string> key = std::make_pair(consumerKey, emitterKey);
	return consumerMap_.find(key) != consumerMap_.cend() ? true : false;
}

