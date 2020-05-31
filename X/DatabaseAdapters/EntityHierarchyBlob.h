#ifndef database_adapters_entityhierarchyblob_h
#define database_adapters_entityhierarchyblob_h

#include <string>
#include <boost/property_tree/ptree.hpp>

#include "config.h"

#include "DatabaseAdapters/ITabularizableResource.h"

namespace database_adapters {

class DATABASE_ADAPTERS_DLL_EXPORT EntityHierarchyBlob : public database_adapters::ITabularizableResource
{
public:
	EntityHierarchyBlob();

	EntityHierarchyBlob(const boost::property_tree::ptree& hierarchyTree);
	EntityHierarchyBlob(const std::string& buff);
	EntityHierarchyBlob(std::string&& buff);

	virtual ~EntityHierarchyBlob();

	const boost::property_tree::ptree& GetHierarchyTree();

	void Assign(const char* buff, const size_t n) override;

	void* Data() override;
	const void* Data() const override;

	size_t GetElementSize() const override;

private:
	std::string data_;
	boost::property_tree::ptree hierarchyTree_;
};

} // database_adapters
#endif // database_adapters_entityhierarchyblob_h
