#include "DatabaseAdapters/IPersistableResource.h"

using database_adapters::IPersistableResource;

IPersistableResource::IPersistableResource() = default;
IPersistableResource::~IPersistableResource() = default;

IPersistableResource::IPersistableResource(const IPersistableResource &) = default;
IPersistableResource &IPersistableResource::operator=(const IPersistableResource &) = default;
IPersistableResource::IPersistableResource(IPersistableResource &&) = default;
IPersistableResource &IPersistableResource::operator=(IPersistableResource &&) = default;
