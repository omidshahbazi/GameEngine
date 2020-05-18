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
		void ImageParser::Parse(const ByteBuffer& Buffer, TextureInfo& TextureInfo)
		{
			stbi_set_flip_vertically_on_load(true);

			int32 desiredChannelCount = 0;

			switch (TextureInfo.Format)
			{
			case Texture::Formats::R8:
			case Texture::Formats::R16:
			case Texture::Formats::R32:
				desiredChannelCount = STBI_grey;
				break;

			case Texture::Formats::RGB8:
			case Texture::Formats::RGB16:
			case Texture::Formats::RGB32:
				desiredChannelCount = STBI_rgb;
				break;

			case Texture::Formats::RGBA8:
			case Texture::Formats::RGBA16:
			case Texture::Formats::RGBA32:
				desiredChannelCount = STBI_rgb_alpha;
				break;
			}

			int32 presentChannelCount;
			TextureInfo.Data = stbi_load_from_memory(Buffer.GetBuffer(), Buffer.GetSize(), &TextureInfo.Dimension.X, &TextureInfo.Dimension.Y, &presentChannelCount, desiredChannelCount);

			if (desiredChannelCount < presentChannelCount)
			{
				//TODO: Use Texture::Format to convert
			}
		}
	}
}