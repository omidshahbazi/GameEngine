// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\RenderingAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			DynamicSizeAllocator RenderingAllocators::RenderingSystemAllocator("Rendering System Allocator", RootAllocator::GetInstance(), MegaByte * 500);
			DynamicSizeAllocator RenderingAllocators::ShaderCompilerAllocator("Shader Compiler Allocator", &RenderingSystemAllocator, MegaByte);
			FrameAllocator RenderingAllocators::CommandAllocator("Command Allocator", &RenderingSystemAllocator, MegaByte * 100);
		}
	}
}