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
				static DynamicSizeAllocator renderingSystemAllocator("Rendering System Allocator", RootAllocator::GetInstance());
				RenderingSystemAllocator = &renderingSystemAllocator;

				static DynamicSizeAllocator containersAllocator("Rendering Containers Allocator", &renderingSystemAllocator);
				ContainersAllocator = &containersAllocator;

				static DynamicSizeAllocator shaderCompilerAllocator("Shader Compiler Allocator", &renderingSystemAllocator);
				ShaderCompilerAllocator = &shaderCompilerAllocator;

				static FrameAllocator CommandAllocator1("Command Allocator 1", &renderingSystemAllocator);
				static FrameAllocator CommandAllocator2("Command Allocator 2", &renderingSystemAllocator);
				static FrameAllocator CommandAllocator3("Command Allocator 3", &renderingSystemAllocator);
				static FrameAllocator CommandAllocator4("Command Allocator 4", &renderingSystemAllocator);
				static FrameAllocator CommandAllocator5("Command Allocator 5", &renderingSystemAllocator);

				CommandAllocators[0] = &CommandAllocator1;
				CommandAllocators[1] = &CommandAllocator2;
				CommandAllocators[2] = &CommandAllocator3;
				CommandAllocators[3] = &CommandAllocator4;
				CommandAllocators[4] = &CommandAllocator5;
			}
		}
	}
}