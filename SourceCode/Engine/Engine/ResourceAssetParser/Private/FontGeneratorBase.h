// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_GENERATOR_BASE_H
#define FONT_GENERATOR_BASE_H

#include <Containers\Buffer.h>
#include <MathContainers\MathContainers.h>
#include <FontSystem\FontInfo.h>

#include <FreeType\include\ft2build.h>
#include FT_FREETYPE_H

namespace Engine
{
	using namespace Containers;
	using namespace MathContainers;
	using namespace FontSystem;

	namespace ResourceAssetParser
	{
		namespace Private
		{
			class FontGeneratorBase
			{
			public:
				FontGeneratorBase(const ByteBuffer& TTFBuffer);

				virtual	~FontGeneratorBase(void);

				virtual	void Generate(FontInfo& FontInfo) = 0;

				virtual int32 GetGlyphCount(void) const
				{
					return m_Face->num_glyphs;
				}

			protected:
				virtual FT_GlyphSlot GetGlyph(void);

				virtual FT_GlyphSlot GetFirstGlyph(uint32& Index, uint64& CharacterCode);

				virtual FT_GlyphSlot GetNextGlyph(uint32& Index, uint64& CharacterCode);

				virtual void LoadCharacter(uint64 CharacterCode);

				virtual void GetGlyphSize(Vector2F& Size);

				virtual void GetGlyphBearing(Vector2F& Bearing);

				virtual void GetGlyphAdvance(Vector2F& Advance);


			private:
				FT_Library m_FreeTypeLib;
				FT_Face m_Face;
				uint64 m_PrevCharacterCode;
			};
		}
	}
}

#endif