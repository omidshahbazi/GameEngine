// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_ASSET_PARSER_ALLOCATORS_H
#define RESOURCE_ASSET_PARSER_ALLOCATORS_H

#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace ResourceAssetParser
	{
		namespace Private
		{
			class RESOURCEASSETPARSER_API ResourceAssetParserAllocators
			{
			public:
				static DynamicSizeAllocator MeshGeneratorAllocator;
				DEFINE_ALLOCATOR_HELPERS(MeshGeneratorAllocator);
			};
		}
	}
}

#endif