// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_PROPERTY_H
#define META_PROPERTY_H
#include <Reflection\ObjectType.h>
#include <Reflection\Private\ImplementPropertyType.h>
#include <ReflectionGenerator\Private\Specifiers.h>

namespace Engine
{
	using namespace Reflection;
	using namespace Reflection::Private;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			class REFLECTIONGENERATOR_API MetaProperty : public ImplementPropertyType, public Specifiers
			{
			public:
				MetaProperty(ObjectType* TopNest) :
					ImplementPropertyType(TopNest)
				{
				}

				virtual ~MetaProperty(void)
				{
				}

				INLINE String GetUniqueName(void) const
				{
					return (ReinterpretCast(MetaObject*, m_TopNest)->GetUniqueName()) + "_" + m_Name;
				}
			};
		}
	}
}
#endif
