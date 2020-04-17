// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\DataStructureType.h>
#include <Reflection\Private\RuntimeImplementation.h>

namespace Engine
{
	namespace Reflection
	{
		using namespace Private;

		DataStructureType::DataStructureType(Type* TopNest) :
			Type(TopNest)
		{
		}

		void DataStructureType::GetParents(AccessSpecifiers AccessFlags, TypeList& List) const
		{
			for each (const auto parentName in m_ParentsName)
			{
				const DataStructureType* type = RuntimeImplementation::FindDataStructureType(parentName);

				if (type == nullptr)
					continue;

				List.Add(ConstCast(DataStructureType*, type));
			}
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
			for each (auto type in (Access == AccessSpecifiers::Public ? m_PublicFunctions : m_NonPublicFunctions))
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