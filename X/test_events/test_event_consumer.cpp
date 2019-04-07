#include <functional>
#include <typeinfo>

#include "config.h"

#include <gtest/gtest.h>

#include "Events/EventConsumer.h"

namespace
{
std::function<int(void)> SUBSCRIBER = [] () { return 7; };
const std::string SUBSCRIBER_TYPE = typeid(int(void)).name();
}

TEST(EventConsumer, Construct) 
{
	EXPECT_NO_THROW(events::EventConsumer<int(void)> consumer(SUBSCRIBER));
}

TEST(EventConsumer, GetSubscriber) 
{
	events::EventConsumer<int(void)> consumer(SUBSCRIBER);
	EXPECT_NO_THROW(consumer.GetSubscriber());

	std::function<int(void)> subscriber = consumer.GetSubscriber();
	EXPECT_EQ(subscriber(), SUBSCRIBER());
}

TEST(EventConsumer, GetSubscriberType)
{
	events::EventConsumer<int(void)> consumer(SUBSCRIBER);
	
	const std::string subscriberType = consumer.GetSubscriberType();

	EXPECT_EQ(subscriberType, SUBSCRIBER_TYPE);
}
