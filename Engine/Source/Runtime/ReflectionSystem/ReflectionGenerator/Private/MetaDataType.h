// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_DATA_TYPE_H
#define META_DATA_TYPE_H
#include <Reflection\Private\ImplementDataType.h>

namespace Engine
{
	using namespace Reflection::Private;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			class REFLECTIONGENERATOR_API MetaDataType : public ImplementDataType
			{
			public:
				virtual ~MetaDataType(void)
				{
				}

				bool GetIsEmpty(void)
				{
					return (m_ValueType == ValueTypes::None && m_ExtraValueType == String::Empty);
				}
			};
		}
	}
}
#endif
