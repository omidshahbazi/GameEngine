// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\FunctionType.h>

namespace Engine
{
	namespace Reflection
	{
		FunctionType::FunctionType(ObjectType* TopNest) :
			Type(TopNest),
			m_IsConst(false)
		{
		}

		AnyDataType FunctionType::Invoke(void* TargetObject) const
		{
			AnyDataType returnValue;
			InvokeInternal(TargetObject, nullptr, returnValue);
			return returnValue;
		}

		AnyDataType FunctionType::Invoke(void* TargetObject, const AnyDataType& Argument) const
		{
			ArgumentsList args;
			args.Add(Argument);

			AnyDataType returnValue;
			InvokeInternal(TargetObject, &args, returnValue);
			return returnValue;
		}

		AnyDataType FunctionType::Invoke(void* TargetObject, const ArgumentsList& Arguments) const
		{
			AnyDataType returnValue;
			InvokeInternal(TargetObject, &Arguments, returnValue);
			return returnValue;
		}
	}
}