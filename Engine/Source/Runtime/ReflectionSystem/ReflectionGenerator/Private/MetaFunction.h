// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_FUNCTION_H
#define META_FUNCTION_H
#include <Reflection\ObjectType.h>
#include <Reflection\Private\ImplementFunctionType.h>
#include <ReflectionGenerator\Private\MetaObject.h>
#include <ReflectionGenerator\Private\Specifiers.h>

namespace Engine
{
	using namespace Reflection;
	using namespace Reflection::Private;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			class REFLECTIONGENERATOR_API MetaFunction : public ImplementFunctionType, public Specifiers
			{
			public:
				MetaFunction(ObjectType* TopNest) :
					ImplementFunctionType(TopNest)
				{
				}
				virtual ~MetaFunction(void)
				{
				}

				INLINE String GetUniqueName(void) const
				{
					return (ReinterpretCast(MetaObject*, m_TopNest)->GetUniqueName()) + "_" + m_Name;
				}

			protected:
				void InvokeInternal(void* TargetObject, const ArgumentsList* Argumetns, AnyDataType& ReturnValue) const override
				{
				}
			};
		}
	}
}
#endif
