// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <InputSystem\Private\InputSystemAllocators.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	namespace InputSystem
	{
		namespace Private
		{
			CREATOR_DEFINITION(InputSystemAllocators);

			DynamicSizeAllocator* InputSystemAllocators::InputSystemAllocator = nullptr;

			InputSystemAllocators::InputSystemAllocators(void)
			{
				static DynamicSizeAllocator inputSystemAllocator("Input System Allocator", RootAllocator::GetInstance(), 32 * KiloByte);
				InputSystemAllocator = &inputSystemAllocator;
			}
		}
	}
}