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
			m_PublicParentsName(Allocator),
			m_NonPublicParentsName(Allocator),
			m_PublicNestedTypes(Allocator),
			m_NonPublicNestedTypes(Allocator),
			m_PublicFunctions(Allocator),
			m_NonPublicFunctions(Allocator),
			m_PublicProperties(Allocator),
			m_NonPublicProperties(Allocator)
		{
		}

		ObjectType::~ObjectType(void)
		{
			for (auto type : m_PublicNestedTypes)
			{
				Destruct(type);
				DeallocateMemory(m_PublicNestedTypes.GetAllocator(), type);
			}

			for (auto type : m_NonPublicNestedTypes)
			{
				Destruct(type);
				DeallocateMemory(m_NonPublicNestedTypes.GetAllocator(), type);
			}

			for (auto type : m_PublicFunctions)
			{
				Destruct(type);
				DeallocateMemory(m_PublicFunctions.GetAllocator(), type);
			}

			for (auto type : m_NonPublicFunctions)
			{
				Destruct(type);
				DeallocateMemory(m_NonPublicFunctions.GetAllocator(), type);
			}

			for (auto type : m_PublicProperties)
			{
				Destruct(type);
				DeallocateMemory(m_PublicProperties.GetAllocator(), type);
			}

			for (auto type : m_NonPublicProperties)
			{
				Destruct(type);
				DeallocateMemory(m_NonPublicProperties.GetAllocator(), type);
			}
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

			ITERATE(m_PublicParentsName);
			ITERATE(m_NonPublicParentsName);
		}

		void ObjectType::GetNestedTypes(AccessSpecifiers AccessFlags, TypeList& List) const
		{
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
				List.AddRange(m_NonPublicNestedTypes);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
				List.AddRange(m_PublicNestedTypes);
		}

		void ObjectType::GetFunctions(AccessSpecifiers AccessFlags, FunctionTypeList& List) const
		{
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
				List.AddRange(m_NonPublicFunctions);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
				List.AddRange(m_PublicFunctions);
		}

		const FunctionType* const ObjectType::GetFunction(const String& Name, AccessSpecifiers Access) const
		{
			for (auto type : (Access == AccessSpecifiers::Public ? m_PublicFunctions : m_NonPublicFunctions))
				if (type->GetName() == Name)
					return (FunctionType*)&type;

			return nullptr;
		}

		void ObjectType::GetProperties(AccessSpecifiers AccessFlags, PropertyTypeList& List) const
		{
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
				List.AddRange(m_NonPublicProperties);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
				List.AddRange(m_PublicProperties);
		}

		AnyDataType ObjectType::CreateInstance(void) const
		{
			AnyDataType returnValue;
			CreateInstanceInternal(returnValue, nullptr);
			return returnValue;
		}

		AnyDataType ObjectType::CreateInstance(const AnyDataType& Argument) const
		{
			ArgumentsList args;
			args.Add(Argument);

			AnyDataType returnValue;
			CreateInstanceInternal(returnValue, &args);
			return returnValue;
		}

		AnyDataType ObjectType::CreateInstance(const ArgumentsList& Arguments) const
		{
			AnyDataType returnValue;
			CreateInstanceInternal(returnValue, &Arguments);
			return returnValue;
		}
	}
}