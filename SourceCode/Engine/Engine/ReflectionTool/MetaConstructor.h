// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef META_CONSTRUCTOR_H
#define META_CONSTRUCTOR_H
#include <Reflection\Type.h>
#include <Reflection\Parameter.h>

namespace Engine
{
	namespace ReflectionTool
	{
		class REFLECTIONTOOL_API MetaConstructor : public Type
		{
		public:
			MetaConstructor(Type *TopNest) :
				Type(TopNest)
			{
			}
			~MetaConstructor(void)
			{
			}

			INLINE Types GetType(void) const
			{
				return Types::Constructor;
			}

			INLINE void SetName(const String &Value)
			{
				m_Name = Value;
			}

			INLINE void AddParameter(const DataType &DataType, const String &Name)
			{
				m_Parameters.Add(Parameter(DataType, Name));
			}

			INLINE const Parameter::ParametersList &GetParameters(void) const
			{
				return m_Parameters;
			}

			void Invoke(void *TargetObject) const;
			void Invoke(void *TargetObject, const AnyDataType &Argument) const;
			void Invoke(void *TargetObject, const ArgumentsList &Arguments) const;

		protected:
			virtual void InvokeInternal(void *TargetObject, const ArgumentsList *Arguments) const
			{

			}

		protected:
			Parameter::ParametersList m_Parameters;
		};
	}
}
#endif