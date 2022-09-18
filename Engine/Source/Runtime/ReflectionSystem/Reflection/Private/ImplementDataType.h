// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef IMPLEMENT_DATA_TYPE_H
#define IMPLEMENT_DATA_TYPE_H
#include <Reflection\DataType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API ImplementDataType : public DataType
			{
			public:
				ImplementDataType(void);

				virtual ~ImplementDataType(void)
				{
				}

				INLINE void SetValueType(ValueTypes Value)
				{
					m_ValueType = Value;
				}

				INLINE void SetPassType(PassesTypes Value)
				{
					m_PassType = Value;
				}

				INLINE void SetExtraValueType(const String& Value)
				{
					m_ExtraValueType = Value;
				}

				INLINE void SetIsConst(bool Value)
				{
					m_IsConst = Value;
				}

				INLINE void SetIsConstValue(bool Value)
				{
					m_IsConstValue = Value;
				}

				INLINE void SetIsTemplate(bool Value)
				{
					m_IsTemplate = Value;
				}

				INLINE void AddTemplateParameter(const DataType& Value)
				{
					m_TemplateParameters.Add(Value);
				}
			};
		}
	}
}
#endif
