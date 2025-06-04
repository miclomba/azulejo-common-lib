
#include "test_entities/config.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Entities/Entity.h"

using entity::Entity;

using Key = Entity::Key;
using SharedEntity = Entity::SharedEntity;

namespace
{
	const int DEFAULT_VALUE = 7;
	const std::string KEY = "key";
	const std::string MEMBER_KEY = "member_key";
	const std::string NEW_KEY = "newKeyB";

	struct TypeB : public Entity
	{
		TypeB() {}
		int GetDefaultValue() const { return DEFAULT_VALUE; }
		int GetValue() const { return value; }
		void SetValue(const int val) { value = val; }

	private:
		int value{DEFAULT_VALUE};
	};

	struct TypeA : public Entity
	{
		TypeA(const Key &key, const Key &memberKey)
		{
			SetKey(key);
			auto e = std::make_shared<TypeB>();
			e->SetKey(memberKey);
			e->SetValue(DEFAULT_VALUE + 1);
			AggregateMember(e);
		}
		TypeA() {}

		template <class T>
		void AddProtectedMember(std::shared_ptr<T> entity) { AggregateMember(std::move(entity)); };
		void AddProtectedMember(const Key &key) { AggregateMember(key); };
		void RemoveProtectedMember(SharedEntity entity) { RemoveMember(std::move(entity)); };
		void RemoveProtectedMember(const Key &key) { RemoveMember(key); };

		const std::map<Key, SharedEntity> &GetProtectedMembers() const { return GetAggregatedMembers(); };
		std::map<Key, SharedEntity> &GetProtectedMembers() { return GetAggregatedMembers(); };

		template <class T>
		SharedEntity &GetProtectedMember(const Key &key) const { return GetAggregatedMember<T>(key); }
		SharedEntity &GetProtectedMember(const Key &key) const { return GetAggregatedMember(key); }
		const std::vector<Key> GetProtectedMemberKeys() const { return GetAggregatedMemberKeys(); }
		const std::vector<Key> GetProtectedMemberKeysTypeB() const { return GetAggregatedMemberKeys<TypeB>(); }
		const std::vector<Key> GetProtectedMemberKeysTypeA() const { return GetAggregatedMemberKeys<TypeA>(); }
		size_t GetExpectedTypeBCount() const { return 1; }
	};
} // end namespace anonymous

TEST(Entity, Construct)
{
	EXPECT_NO_THROW(TypeB entity);
}

TEST(Entity, MoveConstruct)
{
	TypeA source(KEY, MEMBER_KEY);
	SharedEntity &sharedSourceMember = source.GetProtectedMember(MEMBER_KEY);
	TypeB *sourceMemberPtr = dynamic_cast<TypeB *>(sharedSourceMember.get());
	int sourceMemberValue = sourceMemberPtr->GetValue();

	EXPECT_NE(sourceMemberValue, sourceMemberPtr->GetDefaultValue());

	// move
	TypeA target(std::move(source));
	SharedEntity &sharedTargetMember = target.GetProtectedMember(MEMBER_KEY);
	TypeB *targetMemberPtr = dynamic_cast<TypeB *>(sharedTargetMember.get());
	int targetMemberValue = targetMemberPtr->GetValue();

	EXPECT_EQ(KEY, target.GetKey());
	EXPECT_EQ(MEMBER_KEY, sharedTargetMember->GetKey());
	EXPECT_EQ(sourceMemberPtr, targetMemberPtr);
	EXPECT_EQ(sourceMemberValue, targetMemberValue);
}

TEST(Entity, MoveAssign)
{
	TypeA source(KEY, MEMBER_KEY);
	SharedEntity &sharedSourceMember = source.GetProtectedMember(MEMBER_KEY);
	TypeB *sourceMemberPtr = dynamic_cast<TypeB *>(sharedSourceMember.get());
	int sourceMemberValue = sourceMemberPtr->GetValue();

	// move assign
	TypeA target;
	EXPECT_NO_THROW(target = std::move(source));
	SharedEntity &sharedTargetMember = target.GetProtectedMember(MEMBER_KEY);
	TypeB *targetMemberPtr = dynamic_cast<TypeB *>(sharedTargetMember.get());
	int targetMemberValue = targetMemberPtr->GetValue();

	EXPECT_EQ(KEY, target.GetKey());
	EXPECT_EQ(MEMBER_KEY, sharedTargetMember->GetKey());
	EXPECT_EQ(sourceMemberPtr, targetMemberPtr);
	EXPECT_EQ(sourceMemberValue, targetMemberValue);
}

TEST(Entity, CopyConstruct)
{
	TypeA source(KEY, MEMBER_KEY);
	SharedEntity &sharedSourceMember = source.GetProtectedMember(MEMBER_KEY);
	TypeB *sourceMemberPtr = dynamic_cast<TypeB *>(sharedSourceMember.get());

	EXPECT_NE(sourceMemberPtr->GetValue(), sourceMemberPtr->GetDefaultValue());

	// copy
	TypeA target(source);
	SharedEntity &sharedTargetMember = target.GetProtectedMember(MEMBER_KEY);
	TypeB *targetMemberPtr = dynamic_cast<TypeB *>(sharedTargetMember.get());

	EXPECT_EQ(source.GetKey(), target.GetKey());
	EXPECT_NE(&source.GetProtectedMember(MEMBER_KEY), &target.GetProtectedMember(MEMBER_KEY));
	EXPECT_EQ(sourceMemberPtr->GetValue(), targetMemberPtr->GetValue());
}

TEST(Entity, CopyAssign)
{
	TypeA source(KEY, MEMBER_KEY);
	SharedEntity &sharedSourceMember = source.GetProtectedMember(MEMBER_KEY);
	TypeB *sourceMemberPtr = dynamic_cast<TypeB *>(sharedSourceMember.get());

	EXPECT_NE(sourceMemberPtr->GetValue(), sourceMemberPtr->GetDefaultValue());

	// copy assign
	TypeA target;
	EXPECT_NO_THROW(target = source);
	SharedEntity &sharedTargetMember = target.GetProtectedMember(MEMBER_KEY);
	TypeB *targetMemberPtr = dynamic_cast<TypeB *>(sharedTargetMember.get());

	EXPECT_EQ(source.GetKey(), target.GetKey());
	EXPECT_NE(&source.GetProtectedMember(MEMBER_KEY), &target.GetProtectedMember(MEMBER_KEY));
	EXPECT_EQ(sourceMemberPtr->GetValue(), targetMemberPtr->GetValue());
}

TEST(Entity, GetKey)
{
	TypeA obj(KEY, MEMBER_KEY);
	Key key = obj.GetKey();
	EXPECT_EQ(key, KEY);
}

TEST(Entity, GetKeyFromConst)
{
	const TypeA obj(KEY, MEMBER_KEY);
	Key key = obj.GetKey();
	EXPECT_EQ(key, KEY);
}

TEST(Entity, SetKey)
{
	TypeA obj(KEY, MEMBER_KEY);
	obj.SetKey(MEMBER_KEY);
	Key key = obj.GetKey();
	EXPECT_EQ(key, MEMBER_KEY);
}

TEST(Entity, AggregateMember)
{
	TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = ea.GetProtectedMembers().size();
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, AggregateMemberKey)
{
	TypeA ea;
	EXPECT_NO_THROW(ea.GetProtectedMembers());
	size_t count = ea.GetProtectedMembers().size();
	ea.AddProtectedMember(NEW_KEY);
	EXPECT_EQ(ea.GetProtectedMembers().size(), count + 1);
}

TEST(Entity, AggregateMemberThrowsWhenUsingExistingKeyCase1)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);

	auto e2 = std::make_shared<TypeB>();
	e2->SetKey(NEW_KEY);
	EXPECT_THROW(ea.AddProtectedMember(e2), std::runtime_error);
}

TEST(Entity, AggregateMemberThrowsWhenUsingExistingKeyCase2)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);

	EXPECT_THROW(ea.AddProtectedMember(NEW_KEY), std::runtime_error);
}

TEST(Entity, AggregateMemberThrowsWhenGivenNullptr)
{
	TypeA ea;
	EXPECT_THROW(ea.AddProtectedMember(std::shared_ptr<TypeA>()), std::runtime_error);
}

TEST(Entity, GetAggregatedMemberTyped)
{
	TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMember<TypeB>(MEMBER_KEY));
	SharedEntity member = ea.GetProtectedMember<TypeB>(MEMBER_KEY);
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);
}

TEST(Entity, GetAggregatedMemberTypedThrows)
{
	TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMember<TypeB>(MEMBER_KEY));
	SharedEntity member = ea.GetProtectedMember<TypeB>(MEMBER_KEY);
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);

	EXPECT_THROW(ea.GetProtectedMember<TypeA>(MEMBER_KEY), std::runtime_error);
}

TEST(Entity, GetAggregatedMember)
{
	TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMember(MEMBER_KEY));
	SharedEntity member = ea.GetProtectedMember(MEMBER_KEY);
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);
}

TEST(Entity, GetAggregatedMemberAsReference)
{
	TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMember(MEMBER_KEY));
	SharedEntity &memberReference = ea.GetProtectedMember(MEMBER_KEY);
	EXPECT_EQ(memberReference->GetKey(), MEMBER_KEY);
	memberReference.reset();
	SharedEntity memberCopy = ea.GetProtectedMember(MEMBER_KEY);
	EXPECT_TRUE(!memberCopy);
}

TEST(Entity, GetAggregatedMemberFromConst)
{
	const TypeA ea(KEY, MEMBER_KEY);
	EXPECT_NO_THROW(ea.GetProtectedMember(MEMBER_KEY));
	SharedEntity member = ea.GetProtectedMember(MEMBER_KEY);
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);
}

TEST(Entity, GetAggregatedMemberThrows)
{
	TypeA ea;
	EXPECT_THROW(ea.GetProtectedMember(MEMBER_KEY), std::runtime_error);
}

TEST(Entity, GetAggregatedMembers)
{
	TypeA ea(KEY, MEMBER_KEY);

	EXPECT_NO_THROW(ea.GetProtectedMembers());
	std::map<Key, SharedEntity> &members = ea.GetProtectedMembers();

	auto iter = members.find(MEMBER_KEY);
	SharedEntity member = iter->second;

	EXPECT_EQ(ea.GetExpectedTypeBCount(), members.size());
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);
}

TEST(Entity, GetAggregatedMembersFromConst)
{
	const TypeA ea(KEY, MEMBER_KEY);

	EXPECT_NO_THROW(ea.GetProtectedMembers());
	const std::map<Key, SharedEntity> &members = ea.GetProtectedMembers();

	auto iter = members.find(MEMBER_KEY);
	SharedEntity member = iter->second;

	EXPECT_EQ(ea.GetExpectedTypeBCount(), members.size());
	EXPECT_EQ(member->GetKey(), MEMBER_KEY);
}

TEST(Entity, GetAggregatedMembersKeys)
{
	TypeA ea(KEY, MEMBER_KEY);

	EXPECT_NO_THROW(ea.GetProtectedMemberKeys());
	const std::vector<Key> keys = ea.GetProtectedMemberKeys();

	EXPECT_EQ(ea.GetExpectedTypeBCount(), keys.size());
	EXPECT_GT(ea.GetExpectedTypeBCount(), size_t(0));
	EXPECT_EQ(keys[0], MEMBER_KEY);
}

TEST(Entity, GetAggregatedMembersKeysFromConst)
{
	const TypeA ea(KEY, MEMBER_KEY);

	EXPECT_NO_THROW(ea.GetProtectedMemberKeys());
	const std::vector<Key> keys = ea.GetProtectedMemberKeys();

	EXPECT_EQ(ea.GetExpectedTypeBCount(), keys.size());
	EXPECT_GT(ea.GetExpectedTypeBCount(), size_t(0));
	EXPECT_EQ(keys[0], MEMBER_KEY);
}

TEST(Entity, GetAggregatedMembersKeysWithType)
{
	TypeA ea(KEY, MEMBER_KEY);

	EXPECT_NO_THROW(ea.GetProtectedMemberKeysTypeB());
	const std::vector<Key> typeBKeys = ea.GetProtectedMemberKeysTypeB();

	EXPECT_EQ(ea.GetExpectedTypeBCount(), typeBKeys.size());
	EXPECT_GT(ea.GetExpectedTypeBCount(), size_t(0));
	EXPECT_EQ(typeBKeys[0], MEMBER_KEY);

	const std::vector<Key> typeAKeys = ea.GetProtectedMemberKeysTypeA();
	EXPECT_EQ(size_t(0), typeAKeys.size());
}

TEST(Entity, RemoveMember)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);
	ea.AddProtectedMember(e);
	size_t count = ea.GetProtectedMembers().size();
	EXPECT_NO_THROW(ea.RemoveProtectedMember(e));
	EXPECT_EQ(ea.GetProtectedMembers().size(), count - 1);
}

TEST(Entity, RemoveMemberKey)
{
	TypeA ea;
	ea.AddProtectedMember(NEW_KEY);
	size_t count = ea.GetProtectedMembers().size();
	EXPECT_NO_THROW(ea.RemoveProtectedMember(NEW_KEY));
	EXPECT_EQ(ea.GetProtectedMembers().size(), count - 1);
}

TEST(Entity, RemoveMemberThrowsWhenUsingNonExistingKeyCase1)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);

	EXPECT_THROW(ea.RemoveProtectedMember(e), std::runtime_error);
}

TEST(Entity, RemoveMemberThrowsWhenUsingNonExistingKeyCase2)
{
	TypeA ea;
	auto e = std::make_shared<TypeB>();
	e->SetKey(NEW_KEY);

	EXPECT_THROW(ea.RemoveProtectedMember(NEW_KEY), std::runtime_error);
}

TEST(Entity, RemoveMemberThrowsWhenGivenNullptr)
{
	TypeA ea;
	EXPECT_THROW(ea.RemoveProtectedMember(std::shared_ptr<TypeA>()), std::runtime_error);
}
