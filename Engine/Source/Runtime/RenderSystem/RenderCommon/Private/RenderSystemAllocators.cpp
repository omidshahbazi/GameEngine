// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

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

			RenderSystemAllocators::RenderSystemAllocators(void)
			{
				static DynamicSizeAllocator renderSystemAllocator("Render System Allocator", RootAllocator::GetInstance());
				RenderSystemAllocator = &renderSystemAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> resourceAllocator("Render Resource Allocator", &renderSystemAllocator);
				ResourceAllocator = &resourceAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> containersAllocator("Render Containers Allocator", &renderSystemAllocator);
				ContainersAllocator = &containersAllocator;

				static DynamicSizeAllocator programCompilerAllocator("Program Compiler Allocator", &renderSystemAllocator);
				ProgramCompilerAllocator = &programCompilerAllocator;
			}
		}
	}
}