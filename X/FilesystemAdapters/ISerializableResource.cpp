#include "ISerializableResource.h"

using filesystem_adapters::ISerializableResource;

ISerializableResource::ISerializableResource() = default;
ISerializableResource::~ISerializableResource() = default;
ISerializableResource::ISerializableResource(const ISerializableResource&) = default;
ISerializableResource& ISerializableResource::operator=(const ISerializableResource&) = default;
ISerializableResource::ISerializableResource(ISerializableResource&&) = default;
ISerializableResource& ISerializableResource::operator=(ISerializableResource&&) = default;

