#ifndef filesystem_adapters_iserializableresource_h
#define filesystem_adapters_iserializableresource_h

#include "config.h"

namespace filesystem_adapters
{

class FILESYSTEM_ADAPTERS_DLL_EXPORT ISerializableResource 
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

protected:
	bool IsDirty() const;
	int Checksum() const;

private:
	mutable int checkSum_{-1};
};

} // end filesystem_adapters
#endif // end filesystem_adapters_iserializableresource_h


