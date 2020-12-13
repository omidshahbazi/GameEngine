// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\DataStructureType.h>
#include <Reflection\Private\RuntimeImplementation.h>

namespace Engine
{
	namespace Reflection
	{
		using namespace Private;

		DataStructureType::DataStructureType(AllocatorBase* Allocator, Type* TopNest) :
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

		void DataStructureType::GetParents(AccessSpecifiers AccessFlags, TypeList& List) const
		{
#define ITERATE(ListName) \
			for (const auto parentName : ListName) \
			{ \
				const DataStructureType* type = RuntimeImplementation::FindDataStructureType(parentName); \
				if (type == nullptr) \
					continue; \
				List.Add(ConstCast(DataStructureType*, type)); \
			}

			ITERATE(m_PublicParentsName);
			ITERATE(m_NonPublicParentsName);
		}

		void DataStructureType::GetNestedTypes(AccessSpecifiers AccessFlags, TypeList& List) const
		{
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
				List.AddRange(m_NonPublicNestedTypes);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
				List.AddRange(m_PublicNestedTypes);
		}

		void DataStructureType::GetFunctions(AccessSpecifiers AccessFlags, TypeList& List) const
		{
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
				List.AddRange(m_NonPublicFunctions);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
				List.AddRange(m_PublicFunctions);
		}

		const FunctionType* const DataStructureType::GetFunction(const String& Name, AccessSpecifiers Access) const
		{
			for (auto type : (Access == AccessSpecifiers::Public ? m_PublicFunctions : m_NonPublicFunctions))
				if (type->GetName() == Name)
					return (FunctionType*)&type;

			return nullptr;
		}

		void DataStructureType::GetProperties(AccessSpecifiers AccessFlags, TypeList& List) const
		{
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
				List.AddRange(m_NonPublicProperties);
			if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
				List.AddRange(m_PublicProperties);
		}

		AnyDataType DataStructureType::CreateInstance(void) const
		{
			AnyDataType returnValue;
			CreateInstanceInternal(returnValue, nullptr);
			return returnValue;
		}

		AnyDataType DataStructureType::CreateInstance(const AnyDataType& Argument) const
		{
			ArgumentsList args;
			args.Add(Argument);

			AnyDataType returnValue;
			CreateInstanceInternal(returnValue, &args);
			return returnValue;
		}

		AnyDataType DataStructureType::CreateInstance(const ArgumentsList& Arguments) const
		{
			AnyDataType returnValue;
			CreateInstanceInternal(returnValue, &Arguments);
			return returnValue;
		}
	}
}