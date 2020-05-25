#ifndef database_adapters_itabularizableresource_h
#define database_adapters_itabularizableresource_h

#include "config.h"

#include "Resources/IResource.h"

namespace database_adapters {

class ResourceDetabularizer;
class ResourceTabularizer;

class DATABASE_ADAPTERS_DLL_EXPORT ITabularizableResource : public virtual resource::IResource
{
public:
	friend class ResourceTabularizer;
	friend class ResourceDetabularizer;

public:
	ITabularizableResource();
	virtual ~ITabularizableResource();

	ITabularizableResource(const ITabularizableResource&);
	ITabularizableResource& operator=(const ITabularizableResource&);
	ITabularizableResource(ITabularizableResource&&);
	ITabularizableResource& operator=(ITabularizableResource&&);
};

} // end namespace database_adapters

#endif // database_adapters_itabularizableresource_h

