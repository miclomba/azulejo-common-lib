#ifndef filesystem_adapters_iserializableresource_h
#define filesystem_adapters_iserializableresource_h

#include "config.h"

#include "Resources/IResource.h"

namespace filesystem_adapters
{

class FILESYSTEM_ADAPTERS_DLL_EXPORT ISerializableResource : public virtual resource::IResource 
{
public:
	friend class ResourceSerializer;
	friend class ResourceDeserializer;

	ISerializableResource();
	virtual ~ISerializableResource();

	ISerializableResource(const ISerializableResource&);
	ISerializableResource& operator=(const ISerializableResource&);
	ISerializableResource(ISerializableResource&&);
	ISerializableResource& operator=(ISerializableResource&&);
};

} // end filesystem_adapters
#endif // end filesystem_adapters_iserializableresource_h


