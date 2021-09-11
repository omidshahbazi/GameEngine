// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IMAGE_PARSER_H
#define IMAGE_PARSER_H

#include <RenderDevice\TextureInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace RenderDevice;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API ImageParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, TextureInfo& TextureInfo);

			static void Dump(ByteBuffer& Buffer, const TextureInfo& TextureInfo);
		};
	}
}

#endif