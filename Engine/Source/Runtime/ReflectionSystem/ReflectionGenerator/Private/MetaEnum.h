// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_ENUM_H
#define META_ENUM_H
#include <Reflection\ObjectType.h>
#include <Reflection\Private\ImplementEnumType.h>
#include <ReflectionGenerator\Private\Specifiers.h>

namespace Engine
{
	using namespace Reflection;
	using namespace Reflection::Private;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			class REFLECTIONGENERATOR_API MetaEnum : public ImplementEnumType, public Specifiers
			{
			public:
				MetaEnum(ObjectType* TopNest) :
					ImplementEnumType(TopNest)
				{
				}

				virtual ~MetaEnum(void)
				{
				}
			};
		}
	}
}
#endif
