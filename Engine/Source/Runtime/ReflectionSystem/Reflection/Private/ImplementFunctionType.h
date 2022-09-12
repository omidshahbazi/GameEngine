// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef IMPLEMENT_FUNCTION_TYPE_H
#define IMPLEMENT_FUNCTION_TYPE_H
#include <Reflection\FunctionType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API ImplementFunctionType : public FunctionType
			{
			public:
				ImplementFunctionType(ObjectType* TopNest);
				virtual ~ImplementFunctionType(void)
				{
				}

				INLINE void SetName(const String& Value)
				{
					m_Name = Value;
				}

				INLINE void SetReturnType(const DataType& Value)
				{
					m_ReturnType = Value;
				}

				INLINE void AddParameter(const ParameterType& Parameter)
				{
					m_Parameters.Add(Parameter);
				}

				INLINE void SetIsConst(bool Value)
				{
					m_IsConst = Value;
				}
			};
		}
	}
}
#endif
