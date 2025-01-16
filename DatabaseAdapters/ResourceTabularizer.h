#ifndef database_adapters_resourcetabularizer_h
#define database_adapters_resourcetabularizer_h

#include <string>
#include "Config/filesystem.h"

#include "config.h"

#include "ITabularizableResource.h"
#include "Sqlite.h"

namespace database_adapters {

class DATABASE_ADAPTERS_DLL_EXPORT ResourceTabularizer
{
public:
	virtual ~ResourceTabularizer();

	static ResourceTabularizer* GetInstance();
	static void ResetInstance();

	void Tabularize(const ITabularizableResource& resource, const std::string& key);
	void Untabularize(const std::string& key);

	// database
	void CloseDatabase();
	void OpenDatabase(const Path& dbPath);
	Sqlite& GetDatabase();

private:
	ResourceTabularizer();
	ResourceTabularizer(const ResourceTabularizer&) = delete;
	ResourceTabularizer& operator=(const ResourceTabularizer&) = delete;
	ResourceTabularizer(ResourceTabularizer&&) = delete;
	ResourceTabularizer& operator=(ResourceTabularizer&&) = delete;

	static ResourceTabularizer* instance_;

	Sqlite databaseAdapter_;
};

} // end namespace database_adapters
#endif // database_adapters_resourcetabularizer_h

