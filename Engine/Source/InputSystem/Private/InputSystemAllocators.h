// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CORE_SYSTEM_ALLOCATORS_H
#define CORE_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace InputSystem
	{
		namespace Private
		{
			class INPUTSYSTEM_API InputSystemAllocators
			{
			public:
				CREATOR_DECLARATION(InputSystemAllocators);

			private:
				InputSystemAllocators(void);

			public:
				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(InputSystemAllocator);

			private:
				static DynamicSizeAllocator* InputSystemAllocator;
			};
		}
	}
}

#endif