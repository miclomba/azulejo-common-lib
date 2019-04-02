#include "IEventEmitter.h"

#include <boost/signals2/connection.hpp>

namespace events 
{

IEventEmitter::IEventEmitter() = default;
IEventEmitter::IEventEmitter(const IEventEmitter&) = default;
IEventEmitter& IEventEmitter::operator=(const IEventEmitter&) = default;
IEventEmitter::IEventEmitter(IEventEmitter&&) = default;
IEventEmitter& IEventEmitter::operator=(IEventEmitter&&) = default;
IEventEmitter::~IEventEmitter() = default;

} // end namespace events