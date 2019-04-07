
#include "IResource.h"

namespace resource 
{
IResource::IResource() = default;
IResource::~IResource() = default;
IResource::IResource(const IResource&) = default;
IResource& IResource::operator=(const IResource&) = default;
IResource::IResource(IResource&&) = default;
IResource& IResource::operator=(IResource&&) = default;

bool IResource::IsDirty() const
{
	int check = Checksum();
	if (check != checkSum_)
	{
		checkSum_ = check;
		return true;
	}
	return false;
}

}