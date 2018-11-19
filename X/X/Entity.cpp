#include "Entity.h"

#include <iostream>
#include <queue>

namespace global {

using MemberKeys = Entity::MemberKeys;
using Members = Entity::Members;

Entity::Entity() = default;

Entity::Entity(Entity&&) = default;
Entity::Entity(const Entity& other)
{
	for (auto& e : other.GetMembers())
		GetMembers()[e.first] = std::make_shared<Entity>(*e.second);
}

Entity& Entity::operator=(Entity&&) = default;
Entity& Entity::operator=(const Entity& other)
{
	if (this == &other)
		return *this;

	GetMembers().clear();
	for (auto& e : other.GetMembers())
		GetMembers()[e.first] = std::make_shared<Entity>(*e.second);
}

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