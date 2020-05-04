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
	void LoadSerializationStructure(const std::string& pathToJSON);
	bool HasSerializationStructure() const;
	boost::property_tree::ptree GetSerializationStructure() const;

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
	Sqlite databaseAdapter_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;
	mutable std::map<entity::Entity::Key, std::function<std::unique_ptr<ITabularizableEntity>(void)>> keyToEntityMap_;
};

} // end namespace database_adapters
#endif // database_adapters_entityaggregationdetabularizer_h

