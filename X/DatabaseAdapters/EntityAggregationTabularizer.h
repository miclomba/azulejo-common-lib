#ifndef entity_entityaggregationtabularizer_h
#define entity_entityaggregationtabularizer_h

#include <filesystem>
#include <string>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "ITabularizableEntity.h"

namespace database_adapters {

class DATABASE_ADAPTERS_DLL_EXPORT EntityAggregationTabularizer
{
public:
	virtual ~EntityAggregationTabularizer();

	static EntityAggregationTabularizer* GetInstance();
	static void ResetInstance();

	void Tabularize(const ITabularizableEntity& entity);

	void SetSerializationPath(const std::string& pathToJSON);
	std::string GetSerializationPath() const;

	// database
	void CloseDatabase();
	void OpenDatabase(const std::filesystem::path& dbPath);
	Sqlite& GetDatabase();

private:
	EntityAggregationTabularizer();
	EntityAggregationTabularizer(const EntityAggregationTabularizer&) = delete;
	EntityAggregationTabularizer& operator=(const EntityAggregationTabularizer&) = delete;
	EntityAggregationTabularizer(EntityAggregationTabularizer&&) = delete;
	EntityAggregationTabularizer& operator=(EntityAggregationTabularizer&&) = delete;

	void TabularizeWithParentKey(const ITabularizableEntity& entity, const entity::Entity::Key& parentKey = "");

	static EntityAggregationTabularizer* instance_;

	std::filesystem::path serializationPath_;
	boost::property_tree::ptree serializationStructure_;

	Sqlite databaseAdapter_;
};

} // end namespace database_adapters
#endif // database_adapters_entityaggregationtabularizer_h

