// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\FontParser.h>
#include <ResourceAssetParser\MeshParser.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		void FontParser::Parse(const ByteBuffer& Buffer, FontInfo& FontInfo)
		{
		}

		uint64 FontParser::GetDumpSize(const FontInfo& FontInfo)
		{
			uint64 size = sizeof(int32) + sizeof(uint32);

			for each (const auto & glyphInfo in FontInfo.Glyphs)
			{
				size += sizeof(uint64) + sizeof(Vector2F) + sizeof(Vector2F) + sizeof(Vector2F);

				if (FontInfo.RenderType == Font::RenderTypes::Mesh)
				{
					size += sizeof(uint32) + MeshParser::GetDumpSize(glyphInfo.MeshInfo);
				}
				else if (FontInfo.RenderType == Font::RenderTypes::Texture)
				{

				}
			}

			return size;
		}

		void FontParser::Dump(ByteBuffer& Buffer, const FontInfo& FontInfo)
		{
			Buffer << (int32)FontInfo.RenderType;
			Buffer << FontInfo.Glyphs.GetSize();

			for each (const auto & glyphInfo in FontInfo.Glyphs)
			{
				Buffer << glyphInfo.CharCode;

				Buffer << glyphInfo.Size.X;
				Buffer << glyphInfo.Size.Y;

				Buffer << glyphInfo.Bearing.X;
				Buffer << glyphInfo.Bearing.Y;

				Buffer << glyphInfo.Advance.X;
				Buffer << glyphInfo.Advance.Y;

				if (FontInfo.RenderType == Font::RenderTypes::Mesh)
				{
					uint32 count = glyphInfo.MeshInfo.SubMeshes.GetSize();

					Buffer << count;

					if (count != 0)
						MeshParser::Dump(Buffer, glyphInfo.MeshInfo);
				}
				else if (FontInfo.RenderType == Font::RenderTypes::Texture)
				{

				}
			}
		}
	}
}