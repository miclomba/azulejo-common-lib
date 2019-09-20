
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

#include "config.h"

#include <gtest/gtest.h>

#include "Events/EventChannel.h"
#include "Events/EventConsumer.h"
#include "Events/EventEmitter.h"

using events::EventChannel;
using events::EventConsumer;
using events::EventEmitter;

namespace s2 = boost::signals2;

namespace
{
const std::string EMITTER_KEY = "emitter_key";
const std::string CONSUMER_KEY = "consumer_key";
const std::string INVALID_KEY = "invalid_key";

struct Channel : public EventChannel
{
	bool IsEmitterRegistered(const std::string& emitterKey) const 
	{ 
		return EventChannel::IsEmitterRegistered(emitterKey); 
	};

	bool IsConsumerRegistered(const std::string& consumerKey, const std::string& emitterKey) const 
	{ 
		return EventChannel::IsConsumerRegistered(consumerKey, emitterKey); 
	};
};
} // end namespace

TEST(EventChannel, Construct)
{
	EXPECT_NO_THROW(Channel channel);
}

TEST(EventChannel, CopyConstruct)
{
	Channel channel;
	EXPECT_NO_THROW(Channel copy(channel));
}

TEST(EventChannel, CopyAssign)
{
	Channel channel;
	Channel copy;
	EXPECT_NO_THROW(copy = channel);
}

TEST(EventChannel, MoveConstruct)
{
	Channel channel;
	EXPECT_NO_THROW(Channel copy(std::move(channel)));
}

TEST(EventChannel, MoveAssign)
{
	Channel channel;
	Channel copy;
	EXPECT_NO_THROW(copy = std::move(channel));
}

TEST(EventChannel, RegisterEmitter) {

	Channel channel;
	auto emitter = std::make_shared<EventEmitter<void()>>();

	EXPECT_FALSE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterEmitter(EMITTER_KEY, emitter));
	EXPECT_TRUE(channel.IsEmitterRegistered(EMITTER_KEY));
}

TEST(EventChannel, RegisterEmitterThrows) {

	Channel channel;
	auto emitter = std::make_shared<EventEmitter<void()>>();

	EXPECT_THROW(channel.RegisterEmitter(EMITTER_KEY, nullptr), std::invalid_argument);
	EXPECT_THROW(channel.RegisterEmitter("", emitter), std::invalid_argument);
	EXPECT_THROW(channel.RegisterEmitter("", nullptr), std::invalid_argument);

	EXPECT_NO_THROW(channel.RegisterEmitter(EMITTER_KEY, emitter));
	// register an already registered emitter
	EXPECT_THROW(channel.RegisterEmitter(EMITTER_KEY, emitter), std::runtime_error);
}

TEST(EventChannel, RegisterEmitterWithExistingConsumer) {

	Channel channel;
	
	auto consumer = std::make_shared<EventConsumer<void()>>([]() {});
	channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer);

	auto emitter = std::make_shared<EventEmitter<void()>>();
	const s2::signal<void()>& signal = emitter->Signal();

	EXPECT_TRUE(signal.empty());
	EXPECT_FALSE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterEmitter(EMITTER_KEY, emitter));
	EXPECT_TRUE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_EQ(signal.num_slots(), 1);
}

TEST(EventChannel, RegisterEmitterWithNonExistingConsumer) {

	Channel channel;

	auto consumer = std::make_shared<EventConsumer<void()>>([]() {});
	channel.RegisterConsumer(CONSUMER_KEY, INVALID_KEY, consumer);

	auto emitter = std::make_shared<EventEmitter<void()>>();
	const s2::signal<void()>& signal = emitter->Signal();
	EXPECT_TRUE(signal.empty());

	EXPECT_FALSE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterEmitter(EMITTER_KEY, emitter));
	EXPECT_TRUE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_TRUE(signal.empty());
}

TEST(EventChannel, UnregisterEmitter) {

	Channel channel;
	auto emitter = std::make_shared<EventEmitter<void()>>();

	channel.RegisterEmitter(EMITTER_KEY, emitter);
	EXPECT_TRUE(channel.IsEmitterRegistered(EMITTER_KEY));
	EXPECT_NO_THROW(channel.UnregisterEmitter(EMITTER_KEY));
	EXPECT_FALSE(channel.IsEmitterRegistered(EMITTER_KEY));
}

TEST(EventChannel, UnregisterEmitterThrows) {

	Channel channel;

	EXPECT_THROW(channel.UnregisterEmitter(""), std::invalid_argument);
	EXPECT_THROW(channel.UnregisterEmitter(EMITTER_KEY), std::runtime_error);
}

TEST(EventChannel, RegisterConsumer) {

	Channel channel;

	auto consumer = std::make_shared<EventConsumer<void()>>([]() {});

	EXPECT_FALSE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer));
	EXPECT_TRUE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
}

TEST(EventChannel, RegisterConsumerThrows) {

	Channel channel;

	auto consumer = std::make_shared<EventConsumer<void()>>([]() {});

	EXPECT_THROW(channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, nullptr), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer(CONSUMER_KEY, "", consumer), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer(CONSUMER_KEY, "", nullptr), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer("", EMITTER_KEY, consumer), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer("", EMITTER_KEY, nullptr), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer("", "", consumer), std::invalid_argument);
	EXPECT_THROW(channel.RegisterConsumer("", "", nullptr), std::invalid_argument);

	EXPECT_NO_THROW(channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer));
	// register an already registered consumer
	EXPECT_THROW(channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer), std::runtime_error);
}

TEST(EventChannel, UnregisterConsumer) {

	Channel channel;

	auto consumer = std::make_shared<EventConsumer<void()>>([]() {});

	channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer);
	EXPECT_TRUE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_NO_THROW(channel.UnregisterConsumer(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_FALSE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
}

TEST(EventChannel, UnregisterConsumerThrows) {

	Channel channel;

	EXPECT_THROW(channel.UnregisterConsumer(CONSUMER_KEY, ""), std::invalid_argument);
	EXPECT_THROW(channel.UnregisterConsumer("", EMITTER_KEY), std::invalid_argument);
	EXPECT_THROW(channel.UnregisterConsumer("", ""), std::invalid_argument);

	EXPECT_THROW(channel.UnregisterConsumer(CONSUMER_KEY, EMITTER_KEY), std::runtime_error);
}

TEST(EventChannel, RegisterConsumerWithExistingEmitter) {

	Channel channel;

	auto emitter = std::make_shared<EventEmitter<void()>>();
	channel.RegisterEmitter(EMITTER_KEY, emitter);
	const s2::signal<void()>& signal = emitter->Signal();
	EXPECT_TRUE(signal.empty());

	auto consumer = std::make_shared<EventConsumer<void()>>([]() {});

	EXPECT_FALSE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_NO_THROW(channel.RegisterConsumer(CONSUMER_KEY, EMITTER_KEY, consumer));
	EXPECT_TRUE(channel.IsConsumerRegistered(CONSUMER_KEY, EMITTER_KEY));
	EXPECT_EQ(signal.num_slots(), 1);
}

TEST(EventChannel, RegisterConsumerWithNonExistingEmitter) {

	Channel channel;

	auto consumer = std::make_shared<EventConsumer<void()>>([]() {});

	EXPECT_FALSE(channel.IsConsumerRegistered(CONSUMER_KEY, INVALID_KEY));
	EXPECT_NO_THROW(channel.RegisterConsumer(CONSUMER_KEY, INVALID_KEY, consumer));
	EXPECT_TRUE(channel.IsConsumerRegistered(CONSUMER_KEY, INVALID_KEY));
}
