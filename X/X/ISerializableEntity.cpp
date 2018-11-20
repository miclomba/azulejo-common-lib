#include "ISerializableEntity.h"

#include <queue>

namespace global
{

ISerializableEntity::~ISerializableEntity() = default;

std::ostream& operator<<(std::ostream& to, const ISerializableEntity& from)
{
	std::queue<const ISerializableEntity*> que;
	que.push(&from);

	while (!que.empty())
	{
		auto front = que.front();
		que.pop();
		for (auto& e : front->GetMembers())
		{
			auto se = std::dynamic_pointer_cast<ISerializableEntity>(e.second);
			if (!se)
				throw std::runtime_error("Attempted to serialize a non-serializable entity: " + e.first);
			que.push(se.get());
		}
		to << front->Serialize();
	}
	return to;
}

}
