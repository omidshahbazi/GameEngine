// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef META_ENUM_H
#define META_ENUM_H
#include <Reflection\Private\ImplementEnumType.h>
#include <ReflectionTool\Specifiers.h>
#include <ReflectionTool\Token.h>

namespace Engine
{
	namespace ReflectionTool
	{
		class MetaEnum : public ImplementEnumType, public Specifiers
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
