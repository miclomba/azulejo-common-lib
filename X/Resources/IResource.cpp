#include "IResource.h"

#include <boost/crc.hpp>

using resource::IResource;

IResource::IResource() = default;
IResource::~IResource() = default;
IResource::IResource(const IResource&) = default;
IResource& IResource::operator=(const IResource&) = default;
IResource::IResource(IResource&&) = default;
IResource& IResource::operator=(IResource&&) = default;

void IResource::SetColumnSize(const size_t size)
{
	if (M_ != 0 && M_ != size)
		throw std::runtime_error("Cannot change dimensions of Resource object");
	M_ = size;
}

void IResource::SetRowSize(const size_t size)
{
	if (N_ != 0 && N_ != size)
		throw std::runtime_error("Cannot change dimensions of Resource object");
	N_ = size;
}

size_t IResource::GetColumnSize() const
{
	return M_;
}

size_t IResource::GetRowSize() const
{
	return N_;
}

bool IResource::GetDirty() const
{
	return dirty_;
}

bool IResource::UpdateChecksum() const
{
	int check = Checksum();
	if (check == checkSum_)
	{
		dirty_ = false;
		return false;
	}

	checkSum_ = check;
	dirty_ = true;

	return true;
}

int IResource::Checksum() const
{
	size_t size = GetElementSize() * GetColumnSize() * GetRowSize();
	boost::crc_32_type crc;
	crc.process_bytes(Data(), size);
	return crc.checksum();
}
