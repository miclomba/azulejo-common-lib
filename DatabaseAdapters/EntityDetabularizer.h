#ifndef database_adapters_entitydetabularizer_h
#define database_adapters_entitydetabularizer_h

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "Config/filesystem.h"

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "Entities/EntityRegistry.h"
#include "ITabularizableEntity.h"
#include "Sqlite.h"

namespace database_adapters {

class DATABASE_ADAPTERS_DLL_EXPORT EntityDetabularizer
{
public:
	virtual ~EntityDetabularizer();

	static EntityDetabularizer* GetInstance();
	static void ResetInstance();

	//registration
	entity::EntityRegistry<ITabularizableEntity>& GetRegistry();

	// structure
	entity::EntityHierarchy& GetHierarchy();

	// database
	void CloseDatabase();
	void OpenDatabase(const Path& dbPath);
	Sqlite& GetDatabase();

	// detabularization
	void LoadEntity(ITabularizableEntity& entity);

private:
	EntityDetabularizer();
	EntityDetabularizer(const EntityDetabularizer&) = delete;
	EntityDetabularizer& operator=(const EntityDetabularizer&) = delete;
	EntityDetabularizer(EntityDetabularizer&&) = delete;
	EntityDetabularizer& operator=(EntityDetabularizer&&) = delete;

	void LoadWithParentKey(ITabularizableEntity& entity, const entity::Entity::Key& parentKey = "");

	static EntityDetabularizer* instance_;

	entity::EntityRegistry<ITabularizableEntity> registry_;
	entity::EntityHierarchy hierarchy_;
	Sqlite databaseAdapter_;
};

} // end namespace database_adapters
#endif // database_adapters_entitydetabularizer_h

