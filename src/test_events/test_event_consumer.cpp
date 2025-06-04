#include <functional>
#include <typeinfo>
#include <string>

#include "test_events/config.h"

#include <gtest/gtest.h>

#include "Events/EventConsumer.h"

using events::EventConsumer;

namespace
{
	std::function<int(void)> SUBSCRIBER = []()
	{ return 7; };
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
	EXPECT_EQ(source.GetSubscriberType(), target.GetSubscriberType());
}

TEST(EventConsumer, CopyAssign)
{
	EventConsumer<int(void)> source(SUBSCRIBER);
	EventConsumer<int(void)> target(SUBSCRIBER);
	EXPECT_NO_THROW(target = source);
	EXPECT_EQ(source.GetSubscriber().target<int(void)>(), target.GetSubscriber().target<int(void)>());
	EXPECT_EQ(source.GetSubscriberType(), target.GetSubscriberType());
}

TEST(EventConsumer, MoveConstruct)
{
	EventConsumer<int(void)> source(SUBSCRIBER);
	auto sourceFunction = source.GetSubscriber().target<int(void)>();
	std::string sourceFunctionType = source.GetSubscriberType();
	EventConsumer<int(void)> target(std::move(source));
	EXPECT_EQ(sourceFunction, target.GetSubscriber().target<int(void)>());
	EXPECT_EQ(sourceFunctionType, target.GetSubscriberType());
}

TEST(EventConsumer, MoveAssign)
{
	EventConsumer<int(void)> source(SUBSCRIBER);
	auto sourceFunction = source.GetSubscriber().target<int(void)>();
	std::string sourceFunctionType = source.GetSubscriberType();
	EventConsumer<int(void)> target(SUBSCRIBER);
	EXPECT_NO_THROW(target = std::move(source));
	EXPECT_EQ(sourceFunction, target.GetSubscriber().target<int(void)>());
	EXPECT_EQ(sourceFunctionType, target.GetSubscriberType());
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
