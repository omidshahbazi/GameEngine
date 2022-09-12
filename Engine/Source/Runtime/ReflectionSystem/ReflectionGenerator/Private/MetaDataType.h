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
			};
		}
	}
}
#endif
