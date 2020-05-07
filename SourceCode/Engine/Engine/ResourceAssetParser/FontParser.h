// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_PARSER_H
#define FONT_PARSER_H

#include <Containers\Buffer.h>
#include <FontSystem\FontInfo.h>

namespace Engine
{
	using namespace Containers;
	using namespace FontSystem;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API FontParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, FontInfo& FontInfo);

			static uint64 GetDumpSize(const FontInfo& MeshInfo);
			static void Dump(ByteBuffer& Buffer, const  FontInfo& FontInfo);
		};
	}
}

#endif