#include "Events/IEventEmitter.h"

#include <boost/signals2/connection.hpp>

using events::IEventEmitter;

IEventEmitter::IEventEmitter() = default;
IEventEmitter::IEventEmitter(const IEventEmitter &) = default;
IEventEmitter &IEventEmitter::operator=(const IEventEmitter &) = default;
IEventEmitter::IEventEmitter(IEventEmitter &&) = default;
IEventEmitter &IEventEmitter::operator=(IEventEmitter &&) = default;
IEventEmitter::~IEventEmitter() = default;
