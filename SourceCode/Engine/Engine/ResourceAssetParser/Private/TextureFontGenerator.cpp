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
			void TextureFontGenerator::Generate(FontInfo& FontInfo)
			{
				FontInfo::GlyphInfo glyphInfo;

				uint32 glyphIndex;
				GetFirstGlyph(glyphIndex, glyphInfo.CharCode);

				while (glyphIndex != 0)
				{
					LoadCharacterBitmap(glyphInfo.CharCode);

					GetGlyphSize(glyphInfo.Size);
					GetGlyphBearing(glyphInfo.Bearing);
					GetGlyphAdvance(glyphInfo.Advance);

					FontInfo.TextureInfo.Format = Texture::Formats::R8;
					FontInfo.TextureInfo.Type = Texture::Types::TwoD;
					
					FontInfo.TextureInfo.Data = GetBitmapData();
					FontInfo.TextureInfo.Dimension = GetBitmapDimension();



					FontInfo.Glyphs.Add(glyphInfo);

					GetNextGlyph(glyphIndex, glyphInfo.CharCode);

					if (glyphInfo.CharCode == 'T')
						break;
				}
			}
		}
	}
}