// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef FUNCTION_TYPE_H
#define FUNCTION_TYPE_H
#include <Reflection\Type.h>
#include <Reflection\Parameter.h>

namespace Engine
{
	namespace Reflection
	{
		class REFLECTION_API FunctionType : public Type
		{
		public:
			FunctionType(Type *TopNest) :
				Type(TopNest),
				m_IsConst(false)
			{
			}
			virtual ~FunctionType(void)
			{
			}

			INLINE Types GetType(void) const
			{
				return Types::Function;
			}

			INLINE const DataType &GetReturnType(void) const
			{
				return m_ReturnType;
			}

			INLINE const Parameter::ParametersList &GetParameters(void) const
			{
				return m_Parameters;
			}

			INLINE bool GetIsConst(void) const
			{
				return m_IsConst;
			}

			AnyDataType Invoke(void *TargetObject) const;
			AnyDataType Invoke(void *TargetObject, const AnyDataType &Argument) const;
			AnyDataType Invoke(void *TargetObject, const ArgumentsList &Arguments) const;

		protected:
			virtual void InvokeInternal(void *TargetObject, AnyDataType &ReturnValue, const ArgumentsList *Arguments) const = 0;

		protected:
			DataType m_ReturnType;
			Parameter::ParametersList m_Parameters;
			bool m_IsConst;
		};
	}
}
#endif
