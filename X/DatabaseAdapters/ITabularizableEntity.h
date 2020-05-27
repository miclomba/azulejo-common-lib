#ifndef database_adapters_itabularizableentity_h
#define database_adapters_itabularizableentity_h

#include <map>
#include <memory>
#include <string>

#include "config.h"

#include <boost/property_tree/ptree.hpp>

#include "Entities/Entity.h"

namespace database_adapters {

class EntityDetabularizer;
class EntityTabularizer;
class Sqlite;

class DATABASE_ADAPTERS_DLL_EXPORT ITabularizableEntity : public virtual entity::Entity
{
public:
	friend class EntityTabularizer;
	friend class EntityDetabularizer;

public:
	ITabularizableEntity();
	virtual ~ITabularizableEntity();

	ITabularizableEntity(const ITabularizableEntity&);
	ITabularizableEntity& operator=(const ITabularizableEntity&);
	ITabularizableEntity(ITabularizableEntity&&);
	ITabularizableEntity& operator=(ITabularizableEntity&&);

	virtual void Save(boost::property_tree::ptree& tree, Sqlite& database) const = 0;
	virtual void Load(boost::property_tree::ptree& tree, Sqlite& database) = 0;

protected:
	SharedEntity& GetAggregatedMember(const Key& key) const override;
};

} // end namespace database_adapters

#endif // database_adapters_itabularizableentity_h

