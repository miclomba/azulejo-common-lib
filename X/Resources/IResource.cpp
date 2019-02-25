
#include "IResource.h"

namespace resource 
{

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