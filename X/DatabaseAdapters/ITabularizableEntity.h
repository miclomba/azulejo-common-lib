#ifndef database_adapters_itabularizableentity_h
#define database_adapters_itabularizableentity_h

#include <map>
#include <memory>
#include <string>

#include "config.h"

#include "Entities/Entity.h"
#include "Sqlite.h"

namespace database_adapters {

class EntityAggregationDetabularizer;
class EntityAggregationTabularizer;

class DATABASE_ADAPTERS_DLL_EXPORT ITabularizableEntity : public entity::Entity
{
public:
	friend class EntityAggregationTabularizer;
	friend class EntityAggregationDetabularizer;

public:
	ITabularizableEntity();
	virtual ~ITabularizableEntity();

	ITabularizableEntity(const ITabularizableEntity&);
	ITabularizableEntity& operator=(const ITabularizableEntity&);
	ITabularizableEntity(ITabularizableEntity&&);
	ITabularizableEntity& operator=(ITabularizableEntity&&);

	virtual void Save(Sqlite& database) const = 0;
	virtual void Load(Sqlite& database) = 0;

protected:
	SharedEntity& GetAggregatedMember(const Key& key) const override;
};

} // end namespace database_adapters

#endif // database_adapters_itabularizableentity_h

