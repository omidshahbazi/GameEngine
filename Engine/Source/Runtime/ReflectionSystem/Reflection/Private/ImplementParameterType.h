// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef IMPLEMENT_PARAMETER_TYPE_H
#define IMPLEMENT_PARAMETER_TYPE_H
#include <Reflection\ParameterType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API ImplementParameterType : public ParameterType
			{
			public:
				ImplementParameterType(void);
				virtual ~ImplementParameterType(void)
				{
				}

				INLINE void SetName(const String& Value)
				{
					m_Name = Value;
				}

				INLINE void SetDataType(const DataType& Value)
				{
					m_DataType = Value;
				}
			};
		}
	}
}
#endif
