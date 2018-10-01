// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\Allocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			DynamicSizeAllocator Allocators::RenderingSystemAllocator("Rendering System Allocator", RootAllocator::GetInstance(), MegaByte);
			DynamicSizeAllocator Allocators::ShaderCompilerAllocator("Shader Compiler Allocator", RootAllocator::GetInstance(), MegaByte);
		}
	}
}