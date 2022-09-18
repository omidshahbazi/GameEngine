// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\ObjectType.h>
#include <Reflection\Private\RuntimeImplementation.h>

namespace Engine
{
	namespace Reflection
	{
		using namespace Private;

		ObjectType::ObjectType(AllocatorBase* Allocator, ObjectType* TopNest) :
			Type(TopNest),
			m_ParentNames(Allocator),
			m_NestedTypes(Allocator),
			m_Functions(Allocator),
			m_Properties(Allocator)
		{
		}

		ObjectType::~ObjectType(void)
		{
			for (auto list : m_NestedTypes)
				for (auto type : list.GetSecond())
				{
					Destruct(type);
					DeallocateMemory(m_NestedTypes.GetAllocator(), type);
				}

			for (auto list : m_Functions)
				for (auto type : list.GetSecond())
				{
					Destruct(type);
					DeallocateMemory(m_Functions.GetAllocator(), type);
				}

			for (auto list : m_Properties)
				for (auto type : list.GetSecond())
				{
					Destruct(type);
					DeallocateMemory(m_Properties.GetAllocator(), type);
				}
		}

		String ObjectType::GetFullQualifiedName(void) const
		{
			String temp = (m_TopNest == nullptr ? m_Namespace : m_TopNest->GetFullQualifiedName());

			return  temp + (temp == String::Empty ? String::Empty : "::") + m_Name;
		}

		void ObjectType::GetParents(AccessSpecifiers AccessFlags, ObjectTypeList& List) const
		{
#define ITERATE(ListName) \
			for (const auto parentName : ListName) \
			{ \
				const auto* type = RuntimeImplementation::FindObjectType(parentName); \
				if (type == nullptr) \
					continue; \
				List.Add(ConstCast(ObjectType*, type)); \
			}

			for (const auto& parents : m_ParentNames)
				if (BitwiseUtils::IsEnabled(AccessFlags, parents.GetFirst()))
					ITERATE(parents.GetSecond());
		}

		void ObjectType::GetNestedTypes(AccessSpecifiers AccessFlags, TypeList& List) const
		{
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) && m_NestedTypes.Contains(AccessSpecifiers::Private))
				List.AddRange(m_NestedTypes[AccessSpecifiers::Private]);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected) && m_NestedTypes.Contains(AccessSpecifiers::Protected))
				List.AddRange(m_NestedTypes[AccessSpecifiers::Protected]);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public) && m_NestedTypes.Contains(AccessSpecifiers::Public))
				List.AddRange(m_NestedTypes[AccessSpecifiers::Public]);
		}

		void ObjectType::GetFunctions(AccessSpecifiers AccessFlags, FunctionTypeList& List) const
		{
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) && m_Functions.Contains(AccessSpecifiers::Private))
				List.AddRange(m_Functions[AccessSpecifiers::Private]);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected) && m_Functions.Contains(AccessSpecifiers::Protected))
				List.AddRange(m_Functions[AccessSpecifiers::Protected]);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public) && m_Functions.Contains(AccessSpecifiers::Public))
				List.AddRange(m_Functions[AccessSpecifiers::Public]);
		}

		void ObjectType::GetProperties(AccessSpecifiers AccessFlags, PropertyTypeList& List) const
		{
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) && m_Properties.Contains(AccessSpecifiers::Private))
				List.AddRange(m_Properties[AccessSpecifiers::Private]);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected) && m_Properties.Contains(AccessSpecifiers::Protected))
				List.AddRange(m_Properties[AccessSpecifiers::Protected]);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public) && m_Properties.Contains(AccessSpecifiers::Public))
				List.AddRange(m_Properties[AccessSpecifiers::Public]);
		}
	}
}