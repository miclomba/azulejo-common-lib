#ifndef global_entity
#define global_entity

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
	virtual ~Entity();

protected:
	const Members& GetMembers() const;
	Members& GetMembers();

	const MemberKeys GetMemberKeys() const;

private:
	Members members_;
};

} // namespace global
#endif // global_entity

