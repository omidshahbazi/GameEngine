// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef IMPLEMENT_FUNCTION_TYPE_H
#define IMPLEMENT_FUNCTION_TYPE_H
#include <Reflection\FunctionType.h>

namespace Engine
{
	namespace Reflection
	{
		class REFLECTION_API ImplementFunctionType : public FunctionType
		{
		public:
			ImplementFunctionType(Type *TopNest);
			virtual ~ImplementFunctionType(void)
			{
			}

			INLINE void SetName(const String &Value)
			{
				m_Name = Value;
			}

			INLINE void SetReturnType(const DataType &Value)
			{
				m_ReturnType = Value;
			}

			INLINE void AddParameter(const DataType &DataType, const String &Name)
			{
				m_Parameters.Add(Parameter(DataType, Name));
			}

			INLINE void SetIsConst(bool Value)
			{
				m_IsConst = Value;
			}
		};
	}
}
#endif
