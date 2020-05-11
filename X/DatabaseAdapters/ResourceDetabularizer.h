#ifndef database_adapters_resourcedetabularizer_h
#define database_adapters_resourcedetabularizer_h

#include <filesystem>

#include "config.h"

#include "ITabularizableResource.h"
#include "Sqlite.h"

namespace database_adapters {

class DATABASE_ADAPTERS_DLL_EXPORT ResourceDetabularizer
{
public:
	virtual ~ResourceDetabularizer();

	static ResourceDetabularizer* GetInstance();
	static void ResetInstance();

	// database
	void CloseDatabase();
	void OpenDatabase(const std::filesystem::path& dbPath);
	Sqlite& GetDatabase();

	// detabularization
	void LoadResource(ITabularizableResource& entity);

private:
	ResourceDetabularizer();
	ResourceDetabularizer(const ResourceDetabularizer&) = delete;
	ResourceDetabularizer& operator=(const ResourceDetabularizer&) = delete;
	ResourceDetabularizer(ResourceDetabularizer&&) = delete;
	ResourceDetabularizer& operator=(ResourceDetabularizer&&) = delete;

	static ResourceDetabularizer* instance_;

	Sqlite databaseAdapter_;
};

} // end namespace database_adapters
#endif // database_adapters_resourcedetabularizer_h

