// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\RenderingAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			DynamicSizeAllocator RenderingAllocators::RenderingSystemAllocator("Rendering System Allocator", RootAllocator::GetInstance(), MegaByte * 100);
			DynamicSizeAllocator RenderingAllocators::ShaderCompilerAllocator("Shader Compiler Allocator", RootAllocator::GetInstance(), MegaByte);
			FrameAllocator RenderingAllocators::CommandAllocator("Command Allocator", RootAllocator::GetInstance(), MegaByte * 1000);
		}
	}
}