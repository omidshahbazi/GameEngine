// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_CONSTRUCTOR_H
#define META_CONSTRUCTOR_H
#include <ReflectionGenerator\Private\MetaFunction.h>

namespace Engine
{
	using namespace Reflection;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			class REFLECTIONGENERATOR_API MetaConstructor : public MetaFunction
			{
			public:
				MetaConstructor(ObjectType* TopNest) :
					MetaFunction(TopNest)
				{
				}

				virtual ~MetaConstructor(void)
				{
				}
			};
		}
	}
}
#endif
