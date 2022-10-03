// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_PARAMETER_H
#define META_PARAMETER_H
#include <Reflection\Private\ImplementParameterType.h>

namespace Engine
{
	using namespace Reflection;
	using namespace Reflection::Private;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			class REFLECTIONGENERATOR_API MetaParameter : public ImplementParameterType
			{
			public:
				MetaParameter(void)
				{
				}

				virtual ~MetaParameter(void)
				{
				}
			};
		}
	}
}
#endif
