// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef META_ENUM_H
#define META_ENUM_H
#include <Reflection\Private\ImplementEnumType.h>
#include <ReflectionTool\Specifiers.h>

namespace Engine
{
	namespace ReflectionTool
	{
		class REFLECTIONTOOL_API MetaEnum : public ImplementEnumType, public Specifiers
		{
		public:
			MetaEnum(void) :
				ImplementEnumType()
			{
			}
			~MetaEnum(void)
			{
			};
		};
	}
}
#endif
