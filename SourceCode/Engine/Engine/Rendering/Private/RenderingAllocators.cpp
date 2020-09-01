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

			DynamicSizeAllocator* RenderingAllocators::ContainersAllocator = nullptr;

			DynamicSizeAllocator* RenderingAllocators::ShaderCompilerAllocator = nullptr;

			FrameAllocator* RenderingAllocators::CommandAllocators[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };

			RenderingAllocators::RenderingAllocators(void)
			{
				static DynamicSizeAllocator renderingSystemAllocator("Rendering System Allocator", RootAllocator::GetInstance(), MegaByte * 1000);
				RenderingSystemAllocator = &renderingSystemAllocator;

				static DynamicSizeAllocator containersAllocator("Rendering Containers Allocator", &renderingSystemAllocator, MegaByte * 500);
				ContainersAllocator = &containersAllocator;

				static DynamicSizeAllocator shaderCompilerAllocator("Shader Compiler Allocator", &renderingSystemAllocator, MegaByte);
				ShaderCompilerAllocator = &shaderCompilerAllocator;

				static FrameAllocator CommandAllocator1("Command Allocator 1", &renderingSystemAllocator, MegaByte);
				static FrameAllocator CommandAllocator2("Command Allocator 2", &renderingSystemAllocator, MegaByte);
				static FrameAllocator CommandAllocator3("Command Allocator 3", &renderingSystemAllocator, MegaByte);
				static FrameAllocator CommandAllocator4("Command Allocator 4", &renderingSystemAllocator, MegaByte);
				static FrameAllocator CommandAllocator5("Command Allocator 5", &renderingSystemAllocator, MegaByte);

				CommandAllocators[0] = &CommandAllocator1;
				CommandAllocators[1] = &CommandAllocator2;
				CommandAllocators[2] = &CommandAllocator3;
				CommandAllocators[3] = &CommandAllocator4;
				CommandAllocators[4] = &CommandAllocator5;
			}
		}
	}
}