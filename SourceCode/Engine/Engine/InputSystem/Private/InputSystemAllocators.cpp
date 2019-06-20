// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <InputSystem\Private\InputSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace InputSystem
	{
		namespace Private
		{
			DynamicSizeAllocator InputSystemAllocators::InputSystemAllocator("Input System Allocator", RootAllocator::GetInstance(), MegaByte);
		}
	}
}