// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\ImageParser.h>

#define STB_IMAGE_IMPLEMENTATION
#include <STB\stb_image.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		void ImageParser::Parse(const ByteBuffer& Buffer, TextureInfo& TextureInfo, bool ImportAlphaChannel)
		{
			stbi_set_flip_vertically_on_load(true);

			int32 desiredChannelCount = STBI_rgb;
			if (ImportAlphaChannel)
				desiredChannelCount = STBI_rgb_alpha;

			TextureInfo.Data = stbi_load_from_memory(Buffer.GetBuffer(), Buffer.GetSize(), &TextureInfo.Dimension.X, &TextureInfo.Dimension.Y, &TextureInfo.ChannelCount, desiredChannelCount);

			TextureInfo.ChannelCount = Mathematics::Min(desiredChannelCount, TextureInfo.ChannelCount);
		}
	}
}