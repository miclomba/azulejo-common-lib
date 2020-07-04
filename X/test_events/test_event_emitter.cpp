#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include "config.h"

#include <gtest/gtest.h>

#include "Events/EventConsumer.h"
#include "Events/EventEmitter.h"

using events::EventConsumer;
using events::EventEmitter;

namespace
{
struct Consumer : public EventConsumer<void(void)>
{
	Consumer() : EventConsumer<void(void)>([this]() { this->count_++; }) {};
	~Consumer() = default;

	int GetCount() const { return count_; }
private:
	int count_ = 0;
};
} // end namespace

TEST(EventEmitterT, Construct)
{
	EXPECT_NO_THROW(EventEmitter<void(void)> emitter);
}

TEST(EventEmitterT, MoveConstruct)
{
	EventEmitter<void(void)> source;
	int arity = source.Signal().arity;
	size_t numSlots = source.Signal().num_slots();
	bool isEmpty = source.Signal().empty();
	std::string subscriberType = source.GetSubscriberType();

	EventEmitter<void(void)> target(std::move(source));
	EXPECT_EQ(arity, target.Signal().arity);
	EXPECT_EQ(numSlots, target.Signal().num_slots());
	EXPECT_EQ(isEmpty, target.Signal().empty());
	EXPECT_EQ(subscriberType, target.GetSubscriberType());
}

TEST(EventEmitterT, MoveAssign)
{
	EventEmitter<void(void)> source;
	int arity = source.Signal().arity;
	size_t numSlots = source.Signal().num_slots();
	bool isEmpty = source.Signal().empty();
	std::string subscriberType = source.GetSubscriberType();

	EventEmitter<void(void)> target;
	EXPECT_NO_THROW(target = std::move(source));
	EXPECT_EQ(arity, target.Signal().arity);
	EXPECT_EQ(numSlots, target.Signal().num_slots());
	EXPECT_EQ(isEmpty, target.Signal().empty());
	EXPECT_EQ(subscriberType, target.GetSubscriberType());
}

TEST(EventEmitterT, Connect) 
{
	auto consumer = std::make_shared<Consumer>();

	EventEmitter<void(void)> emitter;
	boost::signals2::connection conn = emitter.Connect(consumer);
	EXPECT_TRUE(conn.connected());
}

TEST(EventEmitterT, ConnectThrows)
{
	EventEmitter<void(void)> emitter;
	EXPECT_THROW(emitter.Connect(nullptr), std::runtime_error);
}

TEST(EventEmitterT, DisconnectConsumer)
{
	auto consumer = std::make_shared<Consumer>();

	EventEmitter<void(void)> emitter;
	boost::signals2::connection conn = emitter.Connect(consumer);
	EXPECT_TRUE(conn.connected());

	consumer.reset();
	EXPECT_FALSE(conn.connected());
}

TEST(EventEmitterT, DisconnectEmitter)
{
	auto consumer = std::make_shared<Consumer>();

	boost::signals2::connection conn;
	{
		EventEmitter<void(void)> emitter;
		conn = emitter.Connect(consumer);
		EXPECT_TRUE(conn.connected());
	}
	EXPECT_FALSE(conn.connected());
}

TEST(EventEmitterT, GetSubscriberType) 
{
	auto consumer = std::make_shared<Consumer>();
	std::string consumerSubscriberType = consumer->GetSubscriberType();

	EventEmitter<void(void)> emitter;
	std::string emitterSubscriberType = emitter.GetSubscriberType();

	EXPECT_NO_THROW(emitter.Connect(consumer));
	EXPECT_EQ(emitterSubscriberType, consumerSubscriberType);
}

TEST(EventEmitterT, Signal)
{
	auto consumer = std::make_shared<Consumer>();

	EventEmitter<void(void)> emitter;
	emitter.Connect(consumer);

	int count = consumer->GetCount();
	EXPECT_EQ(count, 0);
	EXPECT_NO_THROW(emitter.Signal()());
	EXPECT_EQ(consumer->GetCount(), count + 1);
}
