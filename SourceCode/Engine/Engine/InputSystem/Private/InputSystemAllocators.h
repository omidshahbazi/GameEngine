// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CORE_SYSTEM_ALLOCATORS_H
#define CORE_SYSTEM_ALLOCATORS_H

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
				DEFINE_ALLOCATOR_HELPERS(InputSystemAllocator);

			private:
				static DynamicSizeAllocator InputSystemAllocator;
			};
		}
	}
}

#endif