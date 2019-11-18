#include "IEventConsumer.h"

using events::IEventConsumer;

IEventConsumer::IEventConsumer() = default;
IEventConsumer::~IEventConsumer() = default;
IEventConsumer::IEventConsumer(const IEventConsumer&) = default;
IEventConsumer& IEventConsumer::operator=(const IEventConsumer&) = default;
IEventConsumer::IEventConsumer(IEventConsumer&&) = default;
IEventConsumer& IEventConsumer::operator=(IEventConsumer&&) = default;

