#include <functional>
#include <string>
#include <typeinfo>

#include "config.h"

#include <gtest/gtest.h>

#include "Events/EventEmitter.h"

namespace
{
const std::string KEY = "key";

class Consumer
{
public:
	void operator()() { count_++; }
	int GetCount() const { return count_; }
	std::function<void(void)> GetSubscriber() { return [this]() { (*this)(); }; }
private:
	int count_ = 0;
};
}

TEST(EventEmitter, Connect) 
{
	Consumer consumer;
	int count = consumer.GetCount();

	events::EventEmitter<void(void)> emitter;
	EXPECT_NO_THROW(emitter.Connect(KEY, consumer.GetSubscriber()));

	emitter.Emit();
	EXPECT_EQ(consumer.GetCount(), count + 1);
}

TEST(EventEmitter, ConnectOnSameKey)
{
	Consumer consumer;
	int count = consumer.GetCount();

	events::EventEmitter<void(void)> emitter;
	EXPECT_NO_THROW(emitter.Connect(KEY, consumer.GetSubscriber()));
	EXPECT_NO_THROW(emitter.Connect(KEY, consumer.GetSubscriber()));

	emitter.Emit();
	EXPECT_EQ(consumer.GetCount(), count + 1);
}

TEST(EventEmitter, Disconnect)
{
	Consumer consumer;
	int count = consumer.GetCount();

	events::EventEmitter<void(void)> emitter;
	EXPECT_NO_THROW(emitter.Connect(KEY, consumer.GetSubscriber()));
	EXPECT_NO_THROW(emitter.Disconnect(KEY));

	emitter.Emit();
	EXPECT_EQ(consumer.GetCount(), count);
}

TEST(EventEmitter, ThrowOnDisconnect)
{
	events::EventEmitter<void(void)> emitter;
	EXPECT_THROW(emitter.Disconnect(KEY), std::runtime_error);
}

TEST(EventEmitter, GetSubscriberType) 
{
	Consumer consumer;

	std::function<void(void)> subscriber = consumer.GetSubscriber();
	std::string subscriberType = typeid(*subscriber.target<void(void)>()).name();

	events::EventEmitter<void(void)> emitter;
	emitter.Connect(KEY, subscriber);

	std::string slotType = emitter.GetSubscriberType();
	EXPECT_EQ(slotType, subscriberType);
}

TEST(EventEmitter, Emit)
{
	Consumer consumer;

	events::EventEmitter<void(void)> emitter;
	emitter.Connect(KEY, consumer.GetSubscriber());

	int count = consumer.GetCount();
	EXPECT_EQ(count, 0);
	EXPECT_NO_THROW(emitter.Emit());
	EXPECT_EQ(consumer.GetCount(), count + 1);
}
