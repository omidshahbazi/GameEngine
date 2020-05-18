// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\TextureParser.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		uint64 GetImageSize(const TextureInfo& TextureInfo)
		{
			return TextureInfo.Dimension.X * TextureInfo.Dimension.Y * Texture::GetPixelSize(TextureInfo.Format);
		}

		void TextureParser::Parse(const ByteBuffer& Buffer, TextureInfo& TextureInfo)
		{
			uint64 index = 0;
			TextureInfo.Dimension.X = Buffer.ReadValue<int32>(index);
			index += sizeof(int32);
			TextureInfo.Dimension.Y = Buffer.ReadValue<int32>(index);
			index += sizeof(int32);

			TextureInfo.Format = (Texture::Formats)Buffer.ReadValue<int32>(index);
			index += sizeof(int32);

			TextureInfo.Borders.X = Buffer.ReadValue<int32>(index);
			index += sizeof(int32);
			TextureInfo.Borders.Z = Buffer.ReadValue<int32>(index);
			index += sizeof(int32);
			TextureInfo.Borders.Y = Buffer.ReadValue<int32>(index);
			index += sizeof(int32);
			TextureInfo.Borders.W = Buffer.ReadValue<int32>(index);
			index += sizeof(int32);

			TextureInfo.Data = Buffer.ReadValue(index, GetImageSize(TextureInfo));
		}

		uint64 TextureParser::GetDumpSize(const TextureInfo& TextureInfo)
		{
			//		Width			Height			ChannelCount	BorderRight		BorderLeft		BorderTop		BorderBottom	DataSize
			return sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + GetImageSize(TextureInfo);
		}

		void TextureParser::Dump(ByteBuffer& Buffer, const TextureInfo& TextureInfo)
		{
			Buffer << TextureInfo.Dimension.X;
			Buffer << TextureInfo.Dimension.Y;
			Buffer << (int32)TextureInfo.Format;
			Buffer << TextureInfo.Borders.X;
			Buffer << TextureInfo.Borders.Z;
			Buffer << TextureInfo.Borders.Y;
			Buffer << TextureInfo.Borders.W;

			Buffer.AppendBuffer(TextureInfo.Data, 0, GetImageSize(TextureInfo));
		}
	}
}