#ifndef database_adapters_entityaggregationdetabularizer_h
#define database_adapters_entityaggregationdetabularizer_h

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/EntityHierarchy.h"
#include "Entities/EntityRegistry.h"
#include "ITabularizableEntity.h"
#include "Sqlite.h"

namespace database_adapters {

class DATABASE_ADAPTERS_DLL_EXPORT EntityAggregationDetabularizer
{
public:
	virtual ~EntityAggregationDetabularizer();

	static EntityAggregationDetabularizer* GetInstance();
	static void ResetInstance();

	//registration
	entity::EntityRegistry<ITabularizableEntity>& GetRegistry();

	// structure
	entity::EntityHierarchy& GetHierarchy();

	// database
	void CloseDatabase();
	void OpenDatabase(const std::filesystem::path& dbPath);
	Sqlite& GetDatabase();

	// detabularization
	void LoadEntity(ITabularizableEntity& entity);

private:
	EntityAggregationDetabularizer();
	EntityAggregationDetabularizer(const EntityAggregationDetabularizer&) = delete;
	EntityAggregationDetabularizer& operator=(const EntityAggregationDetabularizer&) = delete;
	EntityAggregationDetabularizer(EntityAggregationDetabularizer&&) = delete;
	EntityAggregationDetabularizer& operator=(EntityAggregationDetabularizer&&) = delete;

	void LoadWithParentKey(ITabularizableEntity& entity, const entity::Entity::Key& parentKey = "");

	static EntityAggregationDetabularizer* instance_;

	entity::EntityRegistry<ITabularizableEntity> registry_;
	entity::EntityHierarchy hierarchy_;
	Sqlite databaseAdapter_;
};

} // end namespace database_adapters
#endif // database_adapters_entityaggregationdetabularizer_h

