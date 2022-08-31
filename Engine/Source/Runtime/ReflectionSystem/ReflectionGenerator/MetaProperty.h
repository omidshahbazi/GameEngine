// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_PROPERTY_H
#define META_PROPERTY_H
#include <Reflection\Private\ImplementPropertyType.h>
#include <ReflectionGenerator\Specifiers.h>

namespace Engine
{
	namespace ReflectionGenerator
	{
		class REFLECTIONGENERATOR_API MetaProperty : public ImplementPropertyType, public Specifiers
		{
		public:
			MetaProperty(Type *TopNest) :
				ImplementPropertyType(TopNest)
			{
			}
			virtual ~MetaProperty(void)
			{
			}
		};
	}
}
#endif
