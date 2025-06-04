#include "Entities/IStreamableEntity.h"

#include "Entities/Entity.h"

#include <fstream>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>

using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
	const char DELIM = ':';
} // end namespace

namespace entity
{

	IStreamableEntity::IStreamableEntity() = default;

	IStreamableEntity::~IStreamableEntity() = default;

	IStreamableEntity::IStreamableEntity(const IStreamableEntity &) = default;
	IStreamableEntity &IStreamableEntity::operator=(const IStreamableEntity &) = default;
	IStreamableEntity::IStreamableEntity(IStreamableEntity &&) = default;
	IStreamableEntity &IStreamableEntity::operator=(IStreamableEntity &&) = default;

	std::string IStreamableEntity::GetDelimeter()
	{
		return std::string(1, DELIM);
	}

	std::ostream &operator<<(std::ostream &to, const IStreamableEntity &from)
	{
		std::queue<const IStreamableEntity *> que;
		que.push(&from);

		while (!que.empty())
		{
			const IStreamableEntity *front = que.front();
			que.pop();
			for (const std::pair<Key, SharedEntity> &e : front->GetAggregatedMembers())
			{
				auto se = std::dynamic_pointer_cast<IStreamableEntity>(e.second);
				if (!se)
					throw std::runtime_error("Attempted to stream a non-streamable entity: " + e.first);
				que.push(se.get());
			}
			to << front->ToString() << DELIM;
		}
		return to;
	}

	std::istream &operator>>(std::istream &from, IStreamableEntity &to)
	{
		std::string line;
		std::getline(from, line);

		to.FromString(line);

		return from;
	}

} // end namespace entity
