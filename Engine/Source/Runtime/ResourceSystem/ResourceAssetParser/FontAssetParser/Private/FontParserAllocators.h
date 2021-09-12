// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_PARSER_ALLOCATORS_H
#define FONT_PARSER_ALLOCATORS_H

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
			class FONTASSETPARSER_API FontParserAllocators
			{
			public:
				CREATOR_DECLARATION(FontParserAllocators);

			private:
				FontParserAllocators(void);

			public:
				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(MeshGeneratorAllocator);

			public:
				static DynamicSizeAllocator* MeshGeneratorAllocator;
			};
		}
	}
}

#endif