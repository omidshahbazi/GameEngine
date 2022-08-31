// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_CONSTRUCTOR_H
#define META_CONSTRUCTOR_H
#include <Reflection\Type.h>
#include <Reflection\Parameter.h>

namespace Engine
{
	namespace ReflectionGenerator
	{
		class REFLECTIONGENERATOR_API MetaConstructor : public Type
		{
		public:
			MetaConstructor(Type *TopNest) :
				Type(TopNest)
			{
			}
			virtual ~MetaConstructor(void)
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

			INLINE const ParameterList &GetParameters(void) const
			{
				return m_Parameters;
			}

		protected:
			ParameterList m_Parameters;
		};
	}
}
#endif
