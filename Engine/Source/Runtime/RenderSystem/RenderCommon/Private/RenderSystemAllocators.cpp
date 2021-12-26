// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	namespace RenderCommon
	{
		namespace Private
		{
			CREATOR_DEFINITION(RenderSystemAllocators);

			DynamicSizeAllocator* RenderSystemAllocators::RenderSystemAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* RenderSystemAllocators::ResourceAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* RenderSystemAllocators::ContainersAllocator = nullptr;
			DynamicSizeAllocator* RenderSystemAllocators::ProgramCompilerAllocator = nullptr;
			DynamicSizeAllocator* RenderSystemAllocators::CommandBufferAllocator = nullptr;

			RenderSystemAllocators::RenderSystemAllocators(void)
			{
				static DynamicSizeAllocator renderSystemAllocator("Render System Allocator", RootAllocator::GetInstance(), GigaByte);
				RenderSystemAllocator = &renderSystemAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> resourceAllocator("Render Resource Allocator", &renderSystemAllocator, 32 * MegaByte);
				ResourceAllocator = &resourceAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> containersAllocator("Render Containers Allocator", &renderSystemAllocator, 512 * MegaByte);
				ContainersAllocator = &containersAllocator;

				static DynamicSizeAllocator programCompilerAllocator("Program Compiler Allocator", &renderSystemAllocator, 32 * MegaByte);
				ProgramCompilerAllocator = &programCompilerAllocator;

				static DynamicSizeAllocator commandBufferAllocator("Command Buffer Allocator", &renderSystemAllocator, 32 * MegaByte);
				CommandBufferAllocator = &commandBufferAllocator;
			}
		}
	}
}