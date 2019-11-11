
#include "IResource.h"

using resource::IResource;

IResource::IResource() = default;
IResource::~IResource() = default;
IResource::IResource(const IResource&) = default;
IResource& IResource::operator=(const IResource&) = default;
IResource::IResource(IResource&&) = default;
IResource& IResource::operator=(IResource&&) = default;

bool IResource::IsDirty() const
{
	int check = Checksum();
	if (check == checkSum_)
		return false;

	checkSum_ = check;
	return true;
}

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