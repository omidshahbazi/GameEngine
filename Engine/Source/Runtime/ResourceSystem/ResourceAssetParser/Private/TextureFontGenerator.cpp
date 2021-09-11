// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\Private\TextureFontGenerator.h>
#include <RenderDevice\TextureInfo.h>
#include <RenderCommon\Enums.h>
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>

namespace Engine
{
	using namespace ResourceAssetParser;
	using namespace MemoryManagement::Allocator;
	using namespace RenderDevice;
	using namespace RenderCommon;

	namespace ResourceAssetParser
	{
		namespace Private
		{
			const float32 GLYPH_PIXEL_HEIGHT = 64;

			TextureFontGenerator::TextureFontGenerator(const ByteBuffer& TTFBuffer) :
				FontGeneratorBase(TTFBuffer, GLYPH_PIXEL_HEIGHT)
			{
			}

			void TextureFontGenerator::Generate(FontInfo& FontInfo)
			{
				FontInfo.Size = GLYPH_PIXEL_HEIGHT;

				FontInfo::GlyphInfo glyphInfo;

				uint32 glyphCount = GetGlyphCount();
				uint64 glyphsPerSide = Math::Ceil(Math::SquareRoot<float32>(glyphCount));
				uint32 atlasWidth = glyphsPerSide * GLYPH_PIXEL_HEIGHT;
				uint64 atlasDataSize = atlasWidth * atlasWidth;

				byte* atlasData = ResourceAssetParserAllocators::MeshGeneratorAllocator_AllocateArray<byte>(atlasDataSize);
				PlatformMemory::Set(atlasData, 0, atlasDataSize);

				FontInfo.TextureInfo.Format = Formats::R8;
				FontInfo.TextureInfo.Type = TextureTypes::TwoD;
				FontInfo.TextureInfo.Data = atlasData;
				FontInfo.TextureInfo.Dimension = Vector2I(atlasWidth, atlasWidth);

				uint32 columnIndex = 0;
				uint32 rowIndex = 0;

				uint32 glyphIndex;
				GetFirstGlyph(glyphIndex, glyphInfo.CharCode);

				while (glyphIndex != 0)
				{
					LoadCharacterBitmap(glyphInfo.CharCode);

					GetGlyphBitmapDimension(glyphInfo.Size);
					GetGlyphBearing(glyphInfo.Bearing);

					GetGlyphAdvance(glyphInfo.Advance);
					glyphInfo.Advance.X = (int32)glyphInfo.Advance.X >> 6;
					glyphInfo.Advance.Y = (int32)glyphInfo.Advance.Y >> 6;

#define NORMALIZE(Value) Value / (float32)atlasWidth
					glyphInfo.Bounds.X = NORMALIZE(columnIndex);
					glyphInfo.Bounds.Y = NORMALIZE(rowIndex);
					glyphInfo.Bounds.Z = NORMALIZE(glyphInfo.Size.X);
					glyphInfo.Bounds.W = NORMALIZE(glyphInfo.Size.Y);
#undef NORMALIZE

					FontInfo.Glyphs.Add(glyphInfo);

					const byte* const glyphBitmapData = GetGlyphBitmapData();

					if (glyphBitmapData != nullptr)
						for (uint32 i = 0; i < glyphInfo.Size.Y; ++i)
							PlatformMemory::Copy(glyphBitmapData, i * glyphInfo.Size.X, atlasData, ((((atlasWidth - 1) - (rowIndex + i)) * atlasWidth) + columnIndex), glyphInfo.Size.X);

					columnIndex += GLYPH_PIXEL_HEIGHT;

					if (columnIndex >= atlasWidth)
					{
						columnIndex = 0;
						rowIndex += GLYPH_PIXEL_HEIGHT;
					}

					GetNextGlyph(glyphIndex, glyphInfo.CharCode);
				}
			}
		}
	}
}