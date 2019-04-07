
#include <memory>
#include <string>

#include "config.h"

#include <gtest/gtest.h>

#include "Events/EventChannel.h"
#include "Events/EventConsumer.h"
#include "Events/EventEmitter.h"

namespace
{
const std::string EMITTER_KEY = "emitter_key";
const std::string CONSUMER_KEY = "consumer_key";
const std::string INVALID_KEY = "invalid_key";

struct Channel : public events::EventChannel
{
	bool IsEmitterRegistered(const std::string& emitterKey) const { return events::EventChannel::IsEmitterRegistered(emitterKey); };
	bool IsConsumerRegistered(const std::string& consumerKey, const std::string& emitterKey) const { return events::EventChannel::IsConsumerRegistered(consumerKey, emitterKey); };
};
}

TEST(EventChannel, ThrowOnRegisterEmitter) {

	Channel channel;
	auto emitter = std::make_shared<events::EventEmitter<void()>>();

	EXPECT_THROW(channel.RegisterEmitter(EMITTER_KEY, nullptr), std::invalid_argument);
	EXPECT_THROW(channel.RegisterEmitter("", emitter), std::invalid_argument);
	EXPECT_THROW(channel.RegisterEmitter("", nullptr), std::invalid_argument);
}

TEST(EventChannel, RegisterEmitter) {

	Channel channel;
	auto emitter = std::make_shared<events::EventEmitter<void()>>();

	EXPECT_FALSE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterEmitter(EMITTER_KEY, emitter));
	EXPECT_TRUE(channel.IsEmitterRegistered(EMITTER_KEY));
}

TEST(EventChannel, RegisterEmitterWithExistingConsumer) {

	Channel channel;
	
	auto consumer = std::make_shared<events::EventConsumer<void()>>([]() {});
	channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer);

	auto emitter = std::make_shared<events::EventEmitter<void()>>();
	auto& signal = emitter->Signal();
	EXPECT_TRUE(signal.empty());

	EXPECT_FALSE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterEmitter(EMITTER_KEY, emitter));
	EXPECT_TRUE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_EQ(signal.num_slots(), 1);
}

TEST(EventChannel, RegisterEmitterWithNonExistingConsumer) {

	Channel channel;

	auto consumer = std::make_shared<events::EventConsumer<void()>>([]() {});
	channel.RegisterConsumer(CONSUMER_KEY, INVALID_KEY, consumer);

	auto emitter = std::make_shared<events::EventEmitter<void()>>();
	auto& signal = emitter->Signal();
	EXPECT_TRUE(signal.empty());

	EXPECT_FALSE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterEmitter(EMITTER_KEY, emitter));
	EXPECT_TRUE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_TRUE(signal.empty());
}

TEST(EventChannel, UnregisterEmitter) {

	Channel channel;
	auto emitter = std::make_shared<events::EventEmitter<void()>>();

	channel.RegisterEmitter(EMITTER_KEY, emitter);
	EXPECT_TRUE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_NO_THROW(channel.UnregisterEmitter(EMITTER_KEY));
	EXPECT_FALSE(channel.IsEmitterRegistered(EMITTER_KEY));
}

TEST(EventChannel, RegisterConsumer) {

	Channel channel;

	auto consumer = std::make_shared<events::EventConsumer<void()>>([]() {});

	EXPECT_FALSE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer));
	EXPECT_TRUE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
}

TEST(EventChannel, ThrowOnRegisterConsumer) {

	Channel channel;

	auto consumer = std::make_shared<events::EventConsumer<void()>>([]() {});

	EXPECT_THROW(channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, nullptr), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer(CONSUMER_KEY, "", consumer), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer(CONSUMER_KEY, "", nullptr), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer("", EMITTER_KEY, consumer), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer("", EMITTER_KEY, nullptr), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer("", "", consumer), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer("", "", nullptr), std::invalid_argument);
}

TEST(EventChannel, RegisterConsumerWithExistingEmitter) {

	Channel channel;

	auto emitter = std::make_shared<events::EventEmitter<void()>>();
	channel.RegisterEmitter(EMITTER_KEY, emitter);
	auto& signal = emitter->Signal();
	EXPECT_TRUE(signal.empty());

	auto consumer = std::make_shared<events::EventConsumer<void()>>([]() {});

	EXPECT_FALSE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer));
	EXPECT_TRUE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_EQ(signal.num_slots(), 1);
}

TEST(EventChannel, RegisterConsumerWithNonExistingEmitter) {

	Channel channel;

	auto emitter = std::make_shared<events::EventEmitter<void()>>();
	auto& signal = emitter->Signal();
	EXPECT_TRUE(signal.empty());

	auto consumer = std::make_shared<events::EventConsumer<void()>>([]() {});

	EXPECT_FALSE(channel.IsConsumerRegistered(CONSUMER_KEY, INVALID_KEY));
	EXPECT_NO_THROW(channel.RegisterConsumer(CONSUMER_KEY, INVALID_KEY, consumer));
	EXPECT_TRUE(channel.IsConsumerRegistered(CONSUMER_KEY, INVALID_KEY));
	EXPECT_TRUE(signal.empty());
}

TEST(EventChannel, UnregisterConsumer) {

	Channel channel;

	auto consumer = std::make_shared<events::EventConsumer<void()>>([]() {});

	channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer);
	EXPECT_TRUE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_NO_THROW(channel.UnregisterConsumer(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_FALSE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
}