// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\Private\FontGeneratorBase.h>
#include <FreeType\ftoutln.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		namespace Private
		{
			const float32 GLYPH_PIXEL_HEIGHT = 72;

			FontGeneratorBase::FontGeneratorBase(const ByteBuffer& TTFBuffer) :
				m_FreeTypeLib(nullptr),
				m_Face(nullptr),
				m_PrevCharacterCode(0)
			{
				FT_Init_FreeType(&m_FreeTypeLib);

				FT_New_Memory_Face(m_FreeTypeLib, TTFBuffer.GetBuffer(), TTFBuffer.GetSize(), 0, &m_Face);

				FT_Set_Pixel_Sizes(m_Face, 0, GLYPH_PIXEL_HEIGHT);
			}

			FontGeneratorBase::~FontGeneratorBase(void)
			{
				if (m_Face != nullptr)
					FT_Done_Face(m_Face);

				FT_Done_FreeType(m_FreeTypeLib);
			}

			FT_GlyphSlot FontGeneratorBase::GetFirstGlyph(uint32& Index, uint64& CharacterCode)
			{
				CharacterCode = FT_Get_First_Char(m_Face, &Index);
				m_PrevCharacterCode = CharacterCode;

				return m_Face->glyph;
			}

			FT_GlyphSlot FontGeneratorBase::GetNextGlyph(uint32& Index, uint64& CharacterCode)
			{
				CharacterCode = FT_Get_Next_Char(m_Face, m_PrevCharacterCode, &Index);
				m_PrevCharacterCode = CharacterCode;

				return m_Face->glyph;
			}

			void FontGeneratorBase::LoadCharacterOutline(uint64 CharacterCode)
			{
				FT_Load_Char(m_Face, CharacterCode, FT_LOAD_DEFAULT);
			}

			void FontGeneratorBase::LoadCharacterBitmap(uint64 CharacterCode)
			{
				FT_Load_Char(m_Face, CharacterCode, FT_LOAD_RENDER);
			}

			void FontGeneratorBase::GetGlyphSize(Vector2F& Size)
			{
				FT_BBox bb;
				FT_Outline_Get_CBox(&m_Face->glyph->outline, &bb);

				Size.X = (bb.xMax - bb.xMin);
				Size.Y = (bb.yMax - bb.yMin);
			}

			void FontGeneratorBase::GetGlyphBearing(Vector2F& Bearing)
			{
				Bearing.X = m_Face->glyph->bitmap_left;
				Bearing.Y = m_Face->glyph->bitmap_top;
			}

			void FontGeneratorBase::GetGlyphAdvance(Vector2F& Advance)
			{
				Advance.X = m_Face->glyph->advance.x;
				Advance.Y = m_Face->glyph->advance.y;
			}
		}
	}
}