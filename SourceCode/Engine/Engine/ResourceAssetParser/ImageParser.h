// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IMAGE_PARSER_H
#define IMAGE_PARSER_H

#include <Rendering\TextureInfo.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		class RESOURCEASSETPARSER_API ImageParser
		{
		public:
			static void Parse(const ByteBuffer& Buffer, TextureInfo& TextureInfo, bool ImportAlphaChannel);

			static void Dump(ByteBuffer& Buffer, const TextureInfo& TextureInfo)
			{
				//TODO: Impl. dump for image format
			}
		};
	}
}

#endif