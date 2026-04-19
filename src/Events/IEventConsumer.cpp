#include "Events/IEventConsumer.h"

using events::IEventConsumer;

IEventConsumer::IEventConsumer() = default;
IEventConsumer::~IEventConsumer() noexcept = default;
IEventConsumer::IEventConsumer(const IEventConsumer &) = default;
IEventConsumer &IEventConsumer::operator=(const IEventConsumer &) = default;
IEventConsumer::IEventConsumer(IEventConsumer &&) noexcept = default;
IEventConsumer &IEventConsumer::operator=(IEventConsumer &&) noexcept = default;
