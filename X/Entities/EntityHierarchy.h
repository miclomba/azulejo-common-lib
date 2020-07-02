#ifndef entity_entityhierarchy_h
#define entity_entityhierarchy_h

#include <string>
#include "Config/filesystem.h"

#include "config.h"

#include <boost/property_tree/ptree.hpp>

namespace entity {

class ENTITY_DLL_EXPORT EntityHierarchy
{
public:
	EntityHierarchy();
	EntityHierarchy(const EntityHierarchy&);
	EntityHierarchy& operator=(const EntityHierarchy&);
	EntityHierarchy(EntityHierarchy&&);
	EntityHierarchy& operator=(EntityHierarchy&&);
	virtual ~EntityHierarchy();

	// structure
	void LoadSerializationStructure(const Path& pathToJSON);
	bool HasSerializationStructure() const;
	boost::property_tree::ptree& GetSerializationStructure();

	void SetSerializationPath(const Path& pathToJSON);
	Path GetSerializationPath() const;

private:
	Path serializationPath_;
	boost::property_tree::ptree serializationStructure_;
};

} // end namespace entity
#endif // entity_entityhierarchy_h

