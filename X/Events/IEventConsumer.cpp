#include "IEventConsumer.h"

namespace events
{

IEventConsumer::IEventConsumer() = default;
IEventConsumer::~IEventConsumer() = default;
IEventConsumer::IEventConsumer(const IEventConsumer&) = default;
IEventConsumer& IEventConsumer::operator=(const IEventConsumer&) = default;
IEventConsumer::IEventConsumer(IEventConsumer&&) = default;
IEventConsumer& IEventConsumer::operator=(IEventConsumer&&) = default;

} // end namespace events
