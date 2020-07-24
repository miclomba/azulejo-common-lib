#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "config.h"

#if defined(USER_TCP_SERVER_MAIN) || defined(USER_REST_SERVER_MAIN)
int fake_main(int argc, char **argv) 
#else
int main(int argc, char **argv) 
#endif
{
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}