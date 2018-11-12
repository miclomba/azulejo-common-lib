#include "gtest/gtest.h"

#include "X/Entity.h"

TEST(EntityTests, CreateEntity) 
{
    EXPECT_NO_THROW(global::Entity());
}