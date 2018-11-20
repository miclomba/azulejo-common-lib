#ifndef global_entity
#define global_entity

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "config.h"

namespace global
{

class X_DLL_EXPORT Entity
{
public:
	using MemberKeys = std::vector<std::string>;
	using Members = std::map<std::string, std::shared_ptr<Entity>>;

public:
	Entity();
	Entity(const Entity& other);
	Entity& operator=(const Entity& other);
	Entity(Entity&& other);
	Entity& operator=(Entity&& other);
	virtual ~Entity();

protected:
	const Members& GetMembers() const;
	Members& GetMembers();

	//virtual void InsertMember(const std::string& key, const Entity& copied);
	//virtual void InsertMember(const std::string& key, Entity&& moved);

	const MemberKeys GetMemberKeys() const;

private:
	Members members_;
};

} // namespace global

#endif // global_entity

