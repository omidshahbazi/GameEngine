// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\DataStructureType.h>
#include <Reflection\Private\RuntimeImplementation.h>

namespace Engine
{
	namespace Reflection
	{
		using namespace Private;

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