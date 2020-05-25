#ifndef database_adapters_resourcedetabularizer_h
#define database_adapters_resourcedetabularizer_h

#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

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
	template<typename T>
	void RegisterResource(const std::string& key, std::function<std::unique_ptr<ITabularizableResource>(void)> constructor);
	void UnregisterResource(const std::string& key);
	void UnregisterAll();
	bool HasTabularizationKey(const std::string& key) const;

	std::unique_ptr<ITabularizableResource> Detabularize(const std::string& key);
	std::unique_ptr<ITabularizableResource> GenerateResource(const std::string& key) const;

private:
	ResourceDetabularizer();
	ResourceDetabularizer(const ResourceDetabularizer&) = delete;
	ResourceDetabularizer& operator=(const ResourceDetabularizer&) = delete;
	ResourceDetabularizer(ResourceDetabularizer&&) = delete;
	ResourceDetabularizer& operator=(ResourceDetabularizer&&) = delete;

	static ResourceDetabularizer* instance_;
	mutable std::map<std::string, std::function<std::unique_ptr<ITabularizableResource>(void)>> keyToResourceMap_;

	Sqlite databaseAdapter_;
};

#include "ResourceDetabularizer.hpp"

} // end namespace database_adapters
#endif // database_adapters_resourcedetabularizer_h

