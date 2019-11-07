
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
	std::vector<int> check = Checksum();
	if (check == checkSum_)
		return false;

	checkSum_ = check;
	return true;
}