// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\Private\TextureFontGenerator.h>
#include <Rendering\TextureInfo.h>

namespace Engine
{
	using namespace ResourceAssetParser;
	using namespace MemoryManagement::Allocator;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		namespace Private
		{
			const float32 GLYPH_PIXEL_HEIGHT = 2048;

			TextureFontGenerator::TextureFontGenerator(const ByteBuffer& TTFBuffer) :
				FontGeneratorBase(TTFBuffer, GLYPH_PIXEL_HEIGHT)
			{
			}

			void TextureFontGenerator::Generate(FontInfo& FontInfo)
			{
				FontInfo::GlyphInfo glyphInfo;

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

					FontInfo.TextureInfo.Format = Texture::Formats::R8;
					FontInfo.TextureInfo.Type = Texture::Types::TwoD;
					FontInfo.TextureInfo.Data = GetGlyphBitmapData();
					FontInfo.TextureInfo.Dimension = Vector2I(glyphInfo.Size.X, glyphInfo.Size.Y);



					FontInfo.Glyphs.Add(glyphInfo);

					GetNextGlyph(glyphIndex, glyphInfo.CharCode);

					if (glyphInfo.CharCode == 'T')
						break;
				}
			}
		}
	}
}