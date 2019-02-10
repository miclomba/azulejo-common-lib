#include "IStreamableEntity.h"

#include <queue>

namespace
{
const char DELIM = ':';
}

namespace entity {

IStreamableEntity::~IStreamableEntity() = default;

std::string IStreamableEntity::GetDelimeter()
{
	return std::string(1,DELIM);
}

std::ostream& operator<<(std::ostream& to, const IStreamableEntity& from)
{
	std::queue<const IStreamableEntity*> que;
	que.push(&from);

	while (!que.empty())
	{
		auto front = que.front();
		que.pop();
		for (auto& e : front->GetAggregatedMembers())
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

std::istream& operator>>(std::istream& from, IStreamableEntity& to)
{
	std::string line;
	std::getline(from, line);

	to.FromString(line);

	return from;
}

} // end namespace entity
