// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\RenderingAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			CREATOR_DEFINITION(RenderingAllocators);

			DynamicSizeAllocator* RenderingAllocators::RenderingSystemAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* RenderingAllocators::ResourceAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* RenderingAllocators::ContainersAllocator = nullptr;
			DynamicSizeAllocator* RenderingAllocators::ProgramCompilerAllocator = nullptr;

			RenderingAllocators::RenderingAllocators(void)
			{
				static DynamicSizeAllocator renderingSystemAllocator("Rendering System Allocator", RootAllocator::GetInstance());
				RenderingSystemAllocator = &renderingSystemAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> resourceAllocator("Rendering Resource Allocator", &renderingSystemAllocator);
				ResourceAllocator = &resourceAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> containersAllocator("Rendering Containers Allocator", &renderingSystemAllocator);
				ContainersAllocator = &containersAllocator;

				static DynamicSizeAllocator programCompilerAllocator("Program Compiler Allocator", &renderingSystemAllocator);
				ProgramCompilerAllocator = &programCompilerAllocator;
			}
		}
	}
}