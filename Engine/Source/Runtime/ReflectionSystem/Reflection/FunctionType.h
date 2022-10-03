// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef REFLECTION_FUNCTION_TYPE_H
#define REFLECTION_FUNCTION_TYPE_H
#include <Reflection\Type.h>
#include <Reflection\ParameterType.h>

namespace Engine
{
	namespace Reflection
	{
		typedef List<AnyDataType> ArgumentsList;

		class REFLECTION_API FunctionType : public Type
		{
		protected:
			FunctionType(ObjectType* TopNest);

		public:
			virtual ~FunctionType(void)
			{
			}

			INLINE const DataType& GetReturnType(void) const
			{
				return m_ReturnType;
			}

			INLINE const ParameterTypeList& GetParameters(void) const
			{
				return m_Parameters;
			}

			INLINE bool GetIsConst(void) const
			{
				return m_IsConst;
			}

			INLINE bool GetIsStatic(void) const
			{
				return m_IsStatic;
			}

			AnyDataType Invoke(void) const;
			AnyDataType Invoke(const AnyDataType& Argument) const;
			AnyDataType Invoke(const ArgumentsList& Arguments) const;

			AnyDataType Invoke(void* TargetObject) const;
			AnyDataType Invoke(void* TargetObject, const AnyDataType& Argument) const;
			AnyDataType Invoke(void* TargetObject, const ArgumentsList& Arguments) const;

		protected:
			virtual void InvokeInternal(void* TargetObject, const ArgumentsList* Arguments, AnyDataType& ReturnValue) const = 0;

		protected:
			DataType m_ReturnType;
			ParameterTypeList m_Parameters;
			bool m_IsConst;
			bool m_IsStatic;
		};

		typedef Vector<FunctionType*> FunctionTypeList;
	}
}
#endif
