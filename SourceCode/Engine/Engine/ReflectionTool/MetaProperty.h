// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef META_PROPERTY_H
#define META_PROPERTY_H
#include <Reflection\Private\ImplementPropertyType.h>
#include <ReflectionTool\Specifiers.h>

namespace Engine
{
	namespace ReflectionTool
	{
		class REFLECTIONTOOL_API MetaProperty : public ImplementPropertyType, public Specifiers
		{
		public:
			MetaProperty(Type *TopNest) :
				ImplementPropertyType(TopNest)
			{
			}
			~MetaProperty(void)
			{
			}
		};
	}
}
#endif
