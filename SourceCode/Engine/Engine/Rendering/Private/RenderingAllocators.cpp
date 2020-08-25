// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\RenderingAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			DynamicSizeAllocator RenderingAllocators::RenderingSystemAllocator("Rendering System Allocator", RootAllocator::GetInstance(), MegaByte * 1000);

			DynamicSizeAllocator RenderingAllocators::ContainersAllocator("Rendering Containers Allocator", &RenderingSystemAllocator, MegaByte * 500);

			DynamicSizeAllocator RenderingAllocators::ShaderCompilerAllocator("Shader Compiler Allocator", &RenderingSystemAllocator, MegaByte);

			FrameAllocator CommandAllocator1("Command Allocator 1", &RenderingAllocators::RenderingSystemAllocator, MegaByte);
			FrameAllocator CommandAllocator2("Command Allocator 2", &RenderingAllocators::RenderingSystemAllocator, MegaByte);
			FrameAllocator CommandAllocator3("Command Allocator 3", &RenderingAllocators::RenderingSystemAllocator, MegaByte);
			FrameAllocator CommandAllocator4("Command Allocator 4", &RenderingAllocators::RenderingSystemAllocator, MegaByte);
			FrameAllocator CommandAllocator5("Command Allocator 5", &RenderingAllocators::RenderingSystemAllocator, MegaByte);

			FrameAllocator *RenderingAllocators::CommandAllocators[] =
			{
				&CommandAllocator1,
				&CommandAllocator2,
				&CommandAllocator3,
				&CommandAllocator4,
				&CommandAllocator5
			};
		}
	}
}