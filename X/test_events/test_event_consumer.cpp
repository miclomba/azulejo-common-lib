#include <functional>
#include <typeinfo>
#include <string>

#include "config.h"

#include <gtest/gtest.h>

#include "Events/EventConsumer.h"

using events::EventConsumer;

namespace
{
std::function<int(void)> SUBSCRIBER = [] () { return 7; };
const std::string SUBSCRIBER_TYPE = typeid(int(void)).name();
} // end namespace

TEST(EventConsumer, Construct) 
{
	EXPECT_NO_THROW(EventConsumer<int(void)> consumer(SUBSCRIBER));
}

TEST(EventConsumer, CopyConstruct)
{
	EventConsumer<int(void)> consumer(SUBSCRIBER);
	EXPECT_NO_THROW(EventConsumer<int(void)> copy(consumer));
}

TEST(EventConsumer, CopyAssign)
{
	EventConsumer<int(void)> consumer(SUBSCRIBER);
	EventConsumer<int(void)> copy(SUBSCRIBER);
	EXPECT_NO_THROW(copy = consumer);
}

TEST(EventConsumer, MoveConstruct)
{
	EventConsumer<int(void)> consumer(SUBSCRIBER);
	EXPECT_NO_THROW(EventConsumer<int(void)> copy(std::move(consumer)));
}

TEST(EventConsumer, MoveAssign)
{
	EventConsumer<int(void)> consumer(SUBSCRIBER);
	EventConsumer<int(void)> copy(SUBSCRIBER);
	EXPECT_NO_THROW(copy = std::move(consumer));
}

TEST(EventConsumer, GetSubscriber) 
{
	EventConsumer<int(void)> consumer(SUBSCRIBER);
	EXPECT_NO_THROW(consumer.GetSubscriber());

	std::function<int(void)> subscriber = consumer.GetSubscriber();
	EXPECT_EQ(subscriber(), SUBSCRIBER());
}

TEST(EventConsumer, GetSubscriberType)
{
	EventConsumer<int(void)> consumer(SUBSCRIBER);
	
	const std::string subscriberType = consumer.GetSubscriberType();

	EXPECT_EQ(subscriberType, SUBSCRIBER_TYPE);
}
