// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\FontParser.h>
#include <ResourceAssetParser\MeshParser.h>
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		using namespace Private;

		void FontParser::Parse(const ByteBuffer& Buffer, FontInfo& FontInfo)
		{
#define READ_VALUE(Type) \
				Buffer.ReadValue<Type>(index);\
				index += sizeof(Type);

			uint64 index = 0;

			FontInfo.RenderType = (Font::RenderTypes)READ_VALUE(int32);

			uint32 glyphCount = READ_VALUE(uint32);

			FontInfo::GlyphInfo glyphInfo;
			for (uint32 i = 0; i < glyphCount; ++i)
			{
				glyphInfo.CharCode = READ_VALUE(uint64);

				glyphInfo.Size.X = READ_VALUE(float32);
				glyphInfo.Size.Y = READ_VALUE(float32);

				glyphInfo.Bearing.X = READ_VALUE(float32);
				glyphInfo.Bearing.Y = READ_VALUE(float32);

				glyphInfo.Advance.X = READ_VALUE(float32);
				glyphInfo.Advance.Y = READ_VALUE(float32);

				if (FontInfo.RenderType == Font::RenderTypes::Mesh)
				{
					glyphInfo.MeshInfo = MeshInfo(&ResourceAssetParserAllocators::MeshGeneratorAllocator);

					uint64 meshDataSize = READ_VALUE(uint64);

					if (meshDataSize != 0)
					{
						const byte* meshData = Buffer.ReadValue(index, meshDataSize);
						index += meshDataSize;

						MeshParser::Parse(ByteBuffer(ConstCast(byte*, meshData), meshDataSize), glyphInfo.MeshInfo);
					}
				}

				FontInfo.Glyphs.Add(glyphInfo);
			}
#undef READ_VALUE
		}

		uint64 FontParser::GetDumpSize(const FontInfo& FontInfo)
		{
			uint64 size = sizeof(int32) + sizeof(uint32);

			for each (const auto & glyphInfo in FontInfo.Glyphs)
			{
				size += sizeof(uint64) + sizeof(Vector2F) + sizeof(Vector2F) + sizeof(Vector2F);

				if (FontInfo.RenderType == Font::RenderTypes::Mesh)
				{
					size += sizeof(uint64) + MeshParser::GetDumpSize(glyphInfo.MeshInfo);
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
					Buffer << MeshParser::GetDumpSize(glyphInfo.MeshInfo);

					MeshParser::Dump(Buffer, glyphInfo.MeshInfo);
				}
				else if (FontInfo.RenderType == Font::RenderTypes::Texture)
				{

				}
			}
		}
	}
}