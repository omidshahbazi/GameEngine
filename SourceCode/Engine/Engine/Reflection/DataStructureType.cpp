// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\DataStructureType.h>

namespace Engine
{
	namespace Reflection
	{
		AnyDataType DataStructureType::CreateInstance(void) const
		{
			AnyDataType returnValue;
			CreateInstanceInternal(returnValue, nullptr);
			return returnValue;
		}


		AnyDataType DataStructureType::CreateInstance(const AnyDataType &Argument) const
		{
			ArgumentsList args;
			args.Add(Argument);

			AnyDataType returnValue;
			CreateInstanceInternal(returnValue, &args);
			return returnValue;
		}


		AnyDataType DataStructureType::CreateInstance(const ArgumentsList &Arguments) const
		{
			AnyDataType returnValue;
			CreateInstanceInternal(returnValue, &Arguments);
			return returnValue;
		}
	}
}