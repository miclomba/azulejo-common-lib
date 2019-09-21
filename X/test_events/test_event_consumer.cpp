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
	EventConsumer<int(void)> source(SUBSCRIBER);
	EXPECT_NO_THROW(EventConsumer<int(void)> target(source));

	EventConsumer<int(void)> target(source);
	EXPECT_EQ(source.GetSubscriber().target<int(void)>(), target.GetSubscriber().target<int(void)>());
}

TEST(EventConsumer, CopyAssign)
{
	EventConsumer<int(void)> source(SUBSCRIBER);
	EventConsumer<int(void)> target(SUBSCRIBER);
	EXPECT_NO_THROW(target = source);
	EXPECT_EQ(source.GetSubscriber().target<int(void)>(), target.GetSubscriber().target<int(void)>());
}

TEST(EventConsumer, MoveConstruct)
{
	EventConsumer<int(void)> source(SUBSCRIBER);
	auto sourceFunction = source.GetSubscriber().target<int(void)>();
	EventConsumer<int(void)> target(std::move(source));
	EXPECT_EQ(sourceFunction, target.GetSubscriber().target<int(void)>());
}

TEST(EventConsumer, MoveAssign)
{
	EventConsumer<int(void)> source(SUBSCRIBER);
	auto sourceFunction = source.GetSubscriber().target<int(void)>();
	EventConsumer<int(void)> target(SUBSCRIBER);
	EXPECT_NO_THROW(target = std::move(source));
	EXPECT_EQ(sourceFunction, target.GetSubscriber().target<int(void)>());
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
