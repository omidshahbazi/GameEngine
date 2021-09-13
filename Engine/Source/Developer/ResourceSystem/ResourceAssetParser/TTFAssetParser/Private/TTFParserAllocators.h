// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TTF_PARSER_ALLOCATORS_H
#define TTF_PARSER_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <Allocators\DynamicSizeAllocator.h>
#include <Allocators\FrameAllocator.h>

namespace Engine
{
	using namespace Allocators;

	namespace ResourceAssetParser
	{
		namespace Private
		{
			class TTFASSETPARSER_API TTFParserAllocators
			{
			public:
				CREATOR_DECLARATION(TTFParserAllocators);

			private:
				TTFParserAllocators(void);

			public:
				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(MeshGeneratorAllocator);

			public:
				static DynamicSizeAllocator* MeshGeneratorAllocator;
			};
		}
	}
}

#endif