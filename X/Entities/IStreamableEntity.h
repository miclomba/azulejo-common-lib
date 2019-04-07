#ifndef entity_istreamableentity_h
#define entity_istreamableentity_h

#include <iostream>
#include <memory>
#include <string>

#include "config.h"
#include "Entity.h"

namespace entity {

class ENTITY_DLL_EXPORT IStreamableEntity : public Entity
{
public:
	IStreamableEntity();
	virtual ~IStreamableEntity();

	IStreamableEntity(const IStreamableEntity&);
	IStreamableEntity& operator=(const IStreamableEntity&);
	IStreamableEntity(IStreamableEntity&&);
	IStreamableEntity& operator=(IStreamableEntity&&);

	virtual std::string ToString() const = 0;
	virtual void FromString(const std::string& serialization) = 0;

	static std::string GetDelimeter();

	friend ENTITY_DLL_EXPORT std::ostream& operator<<(std::ostream& to, const IStreamableEntity& from);
	friend ENTITY_DLL_EXPORT std::istream& operator>>(std::istream& from, IStreamableEntity& to);
};

} // end namespace entity

#endif // entity_istreamableentity_h

