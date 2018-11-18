#include "Entity.h"

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


}