
#include "config.h"

#include <gtest/gtest.h>

#include "Events/EventEmitter.h"


TEST(EventEmitter, Construct) {

	events::EventEmitter<std::string(int)> emitter;


	//emitter.Connect([](int i) {return ""; });
}