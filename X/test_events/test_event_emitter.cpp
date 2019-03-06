
#include "config.h"

#include <gtest/gtest.h>

#include "Events/EventEmitter.h"


TEST(EventEmitter, Construct) {
  EXPECT_NO_THROW(events::EventEmitter());
}