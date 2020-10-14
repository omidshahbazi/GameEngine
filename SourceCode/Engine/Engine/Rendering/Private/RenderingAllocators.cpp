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

			ThreadSafeAllocator<DynamicSizeAllocator>* RenderingAllocators::ContainersAllocator = nullptr;

			DynamicSizeAllocator* RenderingAllocators::ShaderCompilerAllocator = nullptr;

			FrameAllocator* RenderingAllocators::CommandAllocators[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };

			RenderingAllocators::RenderingAllocators(void)
			{
				static DynamicSizeAllocator renderingSystemAllocator("Rendering System Allocator", RootAllocator::GetInstance());
				RenderingSystemAllocator = &renderingSystemAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> containersAllocator("Rendering Containers Allocator", &renderingSystemAllocator);
				ContainersAllocator = &containersAllocator;

				static DynamicSizeAllocator shaderCompilerAllocator("Shader Compiler Allocator", &renderingSystemAllocator);
				ShaderCompilerAllocator = &shaderCompilerAllocator;

				static FrameAllocator commandAllocator1("Command Allocator 1", &renderingSystemAllocator);
				static FrameAllocator commandAllocator2("Command Allocator 2", &renderingSystemAllocator);
				static FrameAllocator commandAllocator3("Command Allocator 3", &renderingSystemAllocator);
				static FrameAllocator commandAllocator4("Command Allocator 4", &renderingSystemAllocator);
				static FrameAllocator commandAllocator5("Command Allocator 5", &renderingSystemAllocator);

				CommandAllocators[0] = &commandAllocator1;
				CommandAllocators[1] = &commandAllocator2;
				CommandAllocators[2] = &commandAllocator3;
				CommandAllocators[3] = &commandAllocator4;
				CommandAllocators[4] = &commandAllocator5;
			}
		}
	}
}