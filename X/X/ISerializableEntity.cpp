#include "ISerializableEntity.h"

#include <queue>

namespace
{
const char DELIM = ':';
}

namespace global
{

ISerializableEntity::~ISerializableEntity() = default;

std::string ISerializableEntity::GetDelimeter()
{
	return std::string(1,DELIM);
}

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
		to << front->Serialize() << DELIM;
	}
	return to;
}

std::istream& operator>>(std::istream& from, ISerializableEntity& to)
{
	std::string line;
	std::getline(from, line);

	to.Deserialize(line);

	return from;
}

}
