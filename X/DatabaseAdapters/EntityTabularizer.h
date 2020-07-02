#ifndef database_adapters_entitytabularizer_h
#define database_adapters_entitytabularizer_h

#include <string>
#include "Config/filesystem.h"

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "ITabularizableEntity.h"
#include "Sqlite.h"

namespace database_adapters {

class DATABASE_ADAPTERS_DLL_EXPORT EntityTabularizer
{
public:
	virtual ~EntityTabularizer();

	static EntityTabularizer* GetInstance();
	static void ResetInstance();

	void Tabularize(const ITabularizableEntity& entity);

	// structure
	entity::EntityHierarchy& GetHierarchy();

	// database
	void CloseDatabase();
	void OpenDatabase(const Path& dbPath);
	Sqlite& GetDatabase();

private:
	EntityTabularizer();
	EntityTabularizer(const EntityTabularizer&) = delete;
	EntityTabularizer& operator=(const EntityTabularizer&) = delete;
	EntityTabularizer(EntityTabularizer&&) = delete;
	EntityTabularizer& operator=(EntityTabularizer&&) = delete;

	void TabularizeWithParentKey(const ITabularizableEntity& entity, const entity::Entity::Key& parentKey = "");

	static EntityTabularizer* instance_;

	entity::EntityHierarchy hierarchy_;

	Sqlite databaseAdapter_;
};

} // end namespace database_adapters
#endif // database_adapters_entitytabularizer_h

