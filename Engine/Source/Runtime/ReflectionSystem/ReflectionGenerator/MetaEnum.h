// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_ENUM_H
#define META_ENUM_H
#include <Reflection\Private\ImplementEnumType.h>
#include <ReflectionGenerator\Specifiers.h>

namespace Engine
{
	namespace ReflectionGenerator
	{
		class REFLECTIONGENERATOR_API MetaEnum : public ImplementEnumType, public Specifiers
		{
		public:
			MetaEnum(void) :
				ImplementEnumType()
			{
			}
			virtual ~MetaEnum(void)
			{
			};
		};
	}
}
#endif
