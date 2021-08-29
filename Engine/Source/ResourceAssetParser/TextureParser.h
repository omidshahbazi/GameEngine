// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_PARSER_H
#define TEXTURE_PARSER_H

#include <Rendering\TextureInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API TextureParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, TextureInfo& TextureInfo);

			static uint64 GetDumpSize(const TextureInfo& TextureInfo);
			static void Dump(ByteBuffer& Buffer, const TextureInfo& TextureInfo);
		};
	}
}

#endif