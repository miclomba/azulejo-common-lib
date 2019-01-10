#include "gtest/gtest.h"

#include "X/Entity.h"
#include "X/EntityAggregationSerializer.h"

namespace
{
}

TEST(EntityAggregationSerializer, GetInstance)
{
	EXPECT_NO_THROW(global::EntityAggregationSerializer::GetInstance());
	auto serializer = global::EntityAggregationSerializer::GetInstance();
	EXPECT_TRUE(serializer);
	EXPECT_NO_THROW(global::EntityAggregationSerializer::ResetInstance());
}

TEST(EntityAggregationSerializer, ResetInstance)
{
	EXPECT_NO_THROW(global::EntityAggregationSerializer::ResetInstance());
}

TEST(EntityAggregationSerializer, LoadSerializationStructure)
{
}

TEST(EntityAggregationSerializer, Serialize)
{
}

TEST(EntityAggregationSerializer, Deserialize)
{
}

TEST(EntityAggregationSerializer, LazyLoadEntity)
{
}
