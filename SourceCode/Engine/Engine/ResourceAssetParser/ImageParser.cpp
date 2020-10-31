// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\ImageParser.h>

#define STB_IMAGE_IMPLEMENTATION
#include <STB\stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <STB\stb_image_write.h>

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

			int32 desiredChannelCount = Texture::GetChannelCount(TextureInfo.Format);

			int32 presentChannelCount;
			TextureInfo.Data = stbi_load_from_memory(Buffer.GetBuffer(), Buffer.GetSize(), &TextureInfo.Dimension.X, &TextureInfo.Dimension.Y, &presentChannelCount, desiredChannelCount);

			//HITODO: Change the quality or dimension
		}

		void ImageParser::Dump(ByteBuffer& Buffer, const TextureInfo& TextureInfo)
		{
			auto func = [](void* Context, void* Data, int32 Size)
			{
				ReinterpretCast(ByteBuffer*, Context)->AppendBuffer(ReinterpretCast(byte*, Data), 0, Size);
			};

			uint8 channelCount = Texture::GetChannelCount(TextureInfo.Format);

			stbi_write_png_to_func(func, &Buffer, TextureInfo.Dimension.X, TextureInfo.Dimension.Y, channelCount, TextureInfo.Data, TextureInfo.Dimension.X * channelCount);
		}
	}
}