// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef REFLECTION_GENERATOR_H
#define REFLECTION_GENERATOR_H
#include <Allocators\AllocatorBase.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Allocators;
	using namespace Containers;

	namespace ReflectionGenerator
	{
		class REFLECTIONGENERATOR_API ReflectionGenerator
		{
		public:
			static bool Generate(AllocatorBase* Allocator, const WString& FilePath, const WString& OutputBaseFileName);
		};

	}
}
#endif