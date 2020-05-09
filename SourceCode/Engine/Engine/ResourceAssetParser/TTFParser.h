// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TTF_PARSER_H
#define TTF_PARSER_H

#include <Containers\Buffer.h>
#include <FontSystem\FontInfo.h>

namespace Engine
{
	using namespace Containers;
	using namespace FontSystem;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API TTFParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, FontInfo& FontInfo, Font::RenderTypes RenderType);
		};
	}
}

#endif