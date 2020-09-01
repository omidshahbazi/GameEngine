// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_ASSET_PARSER_ALLOCATORS_H
#define RESOURCE_ASSET_PARSER_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
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
				CREATOR_DECLARATION(ResourceAssetParserAllocators);

			private:
				ResourceAssetParserAllocators(void);

			public:
				DEFINE_STATIC_ALLOCATOR_HELPERS1(MeshGeneratorAllocator);

			public:
				static DynamicSizeAllocator* MeshGeneratorAllocator;
			};
		}
	}
}

#endif