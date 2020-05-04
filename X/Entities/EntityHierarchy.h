#ifndef entity_entityhierarchy_h
#define entity_entityhierarchy_h

#include <filesystem>
#include <string>

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
	void LoadSerializationStructure(const std::filesystem::path& pathToJSON);
	bool HasSerializationStructure() const;
	boost::property_tree::ptree& GetSerializationStructure();

	void SetSerializationPath(const std::filesystem::path& pathToJSON);
	std::filesystem::path GetSerializationPath() const;

private:
	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;
};

} // end namespace entity
#endif // entity_entityhierarchy_h

