#include "Entity.h"

#include <iostream>
#include <queue>

namespace global {

using MemberKeys = Entity::MemberKeys;
using Members = Entity::Members;

Entity::~Entity() = default;

const Members& Entity::GetMembers() const
{
	return members_;
}

Members& Entity::GetMembers()
{
	return members_;
}

const MemberKeys Entity::GetMemberKeys() const
{
	MemberKeys keys;
	for (auto& e : GetMembers())
		keys.push_back(e.first);
	return keys;
}

std::ostream& operator<<(std::ostream& to, const Entity& from)
{
	std::queue<const Entity*> que;
	que.push(&from);
	
	while (!que.empty())
	{
		auto front = que.front();
		que.pop();
		for (auto& e : front->GetMembers())
			que.push(e.second.get());
		to << front->Serialize();
	}
	return to;
}

}