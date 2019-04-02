#include <functional>
#include <memory>
#include <string>
#include <typeinfo>

#include "config.h"

#include <gtest/gtest.h>

#include "Events/EventConsumer.h"
#include "Events/EventEmitter.h"

namespace
{
class Consumer : public events::EventConsumer<void(void)>
{
public:
	Consumer() : events::EventConsumer<void(void)>([this]() { this->count_++; }) {};
	~Consumer() = default;

	int GetCount() const { return count_; }
private:
	int count_ = 0;
};
}

TEST(EventEmitter, Connect) 
{
	auto consumer = std::make_shared<Consumer>();
	int count = consumer->GetCount();

	events::EventEmitter<void(void)> emitter;
	boost::signals2::connection conn = emitter.Connect(consumer);
	EXPECT_TRUE(conn.connected());
}

TEST(EventEmitter, Disconnect)
{
	auto consumer = std::make_shared<Consumer>();
	int count = consumer->GetCount();

	events::EventEmitter<void(void)> emitter;
	boost::signals2::connection conn = emitter.Connect(consumer);

	consumer.reset();
	EXPECT_FALSE(conn.connected());
}

TEST(EventEmitter, GetSubscriberType) 
{
	auto consumer = std::make_shared<Consumer>();
	std::string consumerSubscriberType = consumer->GetSubscriberType();

	events::EventEmitter<void(void)> emitter;
	std::string emitterSubscriberType = emitter.GetSubscriberType();

	EXPECT_NO_THROW(emitter.Connect(consumer));
	EXPECT_EQ(emitterSubscriberType, consumerSubscriberType);
}

TEST(EventEmitter, Emit)
{
	auto consumer = std::make_shared<Consumer>();

	events::EventEmitter<void(void)> emitter;
	emitter.Connect(consumer);

	int count = consumer->GetCount();
	EXPECT_EQ(count, 0);
	EXPECT_NO_THROW(emitter.Emit());
	EXPECT_EQ(consumer->GetCount(), count + 1);
}
