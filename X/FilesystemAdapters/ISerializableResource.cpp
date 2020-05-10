#include "ISerializableResource.h"

#include <boost/crc.hpp>

using filesystem_adapters::ISerializableResource;

ISerializableResource::ISerializableResource() = default;
ISerializableResource::~ISerializableResource() = default;
ISerializableResource::ISerializableResource(const ISerializableResource&) = default;
ISerializableResource& ISerializableResource::operator=(const ISerializableResource&) = default;
ISerializableResource::ISerializableResource(ISerializableResource&&) = default;
ISerializableResource& ISerializableResource::operator=(ISerializableResource&&) = default;

bool ISerializableResource::IsDirty() const
{
	int check = Checksum();
	if (check == checkSum_)
		return false;

	checkSum_ = check;
	return true;
}

int ISerializableResource::Checksum() const
{
	size_t size = GetElementSize() * GetColumnSize() * GetRowSize();
	boost::crc_32_type crc;
	crc.process_bytes(Data(), size);
	return crc.checksum();
}

