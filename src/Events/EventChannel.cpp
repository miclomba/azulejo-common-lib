#include "Events/EventChannel.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "Events/IEventConsumer.h"
#include "Events/IEventEmitter.h"
#include "Events/EventConsumer.h"

using events::EventChannel;
using events::IEventConsumer;
using events::IEventEmitter;

EventChannel::EventChannel() = default;
EventChannel::~EventChannel() noexcept = default;
EventChannel::EventChannel(const EventChannel &) = default;
EventChannel &EventChannel::operator=(const EventChannel &) = default;
EventChannel::EventChannel(EventChannel &&) noexcept = default;
EventChannel &EventChannel::operator=(EventChannel &&) noexcept = default;

void EventChannel::RegisterEmitter(const std::string_view emitterKey, const std::shared_ptr<IEventEmitter> emitter)
{
	if (emitterKey.empty())
		throw std::invalid_argument("Cannot register emitter with empty key in EventChannel.");
	if (!emitter)
		throw std::invalid_argument("Cannot register NULL emitter in EventChannel.");

	if (IsEmitterRegistered(emitterKey))
		throw std::runtime_error("IEventEmitter with key is already registered in this EventChannel");

	for (auto iter = consumerMap_.begin(); iter != consumerMap_.end(); ++iter)
	{
		auto& [consumerEmitterNamePair, ieventConsumer] = *iter;
		auto& [consumerName, emitterName] = consumerEmitterNamePair;
		if (emitterName == emitterKey && !ieventConsumer.expired())
			emitter->Connect(ieventConsumer.lock());
	}

	emitterMap_[std::string(emitterKey)] = emitter;
}

void EventChannel::UnregisterEmitter(const std::string_view emitterKey)
{
	if (emitterKey.empty())
		throw std::invalid_argument("Cannot unregister emitter with empty key in EventChannel.");

	if (!IsEmitterRegistered(emitterKey))
		throw std::runtime_error("IEventEmitter with key is not registered in this EventChannel");
	emitterMap_.erase(std::string(emitterKey));
}

bool EventChannel::IsEmitterRegistered(const std::string_view emitterKey) const
{
	return emitterMap_.find(emitterKey) != emitterMap_.cend() ? true : false;
}

void EventChannel::RegisterConsumer(const std::string_view consumerKey, const std::string_view emitterKey, const std::shared_ptr<IEventConsumer> consumer)
{
	if (consumerKey.empty() || emitterKey.empty())
		throw std::invalid_argument("Cannot register consumer with empty key in EventChannel.");
	if (!consumer)
		throw std::invalid_argument("Cannot register NULL consumer in EventChannel.");

	if (IsConsumerRegistered(consumerKey, emitterKey))
		throw std::runtime_error("IEventConsumer with key=[" + std::string(consumerKey) + "," + std::string(emitterKey) + "] is already registered in this EventChannel");

	for (auto iter = emitterMap_.begin(); iter != emitterMap_.end(); ++iter)
	{
		auto& [emitterName, ieventEmitter] = *iter;
		if (emitterName == emitterKey && !ieventEmitter.expired())
		{
			std::shared_ptr<IEventEmitter> eventEmitter = ieventEmitter.lock();
			eventEmitter->Connect(consumer);
		}
	}

	auto key = std::make_pair(std::string(consumerKey), std::string(emitterKey));
	consumerMap_[key] = consumer;
}

void EventChannel::UnregisterConsumer(const std::string_view consumerKey, const std::string_view emitterKey)
{
	if (consumerKey.empty() || emitterKey.empty())
		throw std::invalid_argument("Cannot unregister consumer with empty key in EventChannel.");

	auto key = std::make_pair(std::string(consumerKey), std::string(emitterKey));
	if (!IsConsumerRegistered(consumerKey, emitterKey))
		throw std::runtime_error("IEventConsumer with key is not registered in this EventChannel");
	consumerMap_.erase(key);
}

bool EventChannel::IsConsumerRegistered(const std::string_view consumerKey, const std::string_view emitterKey) const
{
	auto key = std::make_pair(std::string(consumerKey), std::string(emitterKey));
	return consumerMap_.find(key) != consumerMap_.cend() ? true : false;
}
