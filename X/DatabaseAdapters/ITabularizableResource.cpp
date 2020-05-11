#include "ITabularizableResource.h"

using database_adapters::ITabularizableResource;

ITabularizableResource::ITabularizableResource() = default;
ITabularizableResource::~ITabularizableResource() = default;

ITabularizableResource::ITabularizableResource(const ITabularizableResource&) = default;
ITabularizableResource& ITabularizableResource::operator=(const ITabularizableResource&) = default;
ITabularizableResource::ITabularizableResource(ITabularizableResource&&) = default;
ITabularizableResource& ITabularizableResource::operator=(ITabularizableResource&&) = default;

