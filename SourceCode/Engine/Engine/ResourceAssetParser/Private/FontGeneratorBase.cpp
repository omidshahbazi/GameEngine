// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\Private\FontGeneratorBase.h>
#include <FreeType\ftoutln.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		namespace Private
		{
			FontGeneratorBase::FontGeneratorBase(const ByteBuffer& TTFBuffer, uint32 PixelSize) :
				m_FreeTypeLib(nullptr),
				m_Face(nullptr),
				m_PrevCharacterCode(0)
			{
				FT_Init_FreeType(&m_FreeTypeLib);

				FT_New_Memory_Face(m_FreeTypeLib, TTFBuffer.GetBuffer(), TTFBuffer.GetSize(), 0, &m_Face);

				FT_Set_Pixel_Sizes(m_Face, 0, PixelSize);
			}

			FontGeneratorBase::~FontGeneratorBase(void)
			{
				//if (m_Face != nullptr)
				//	FT_Done_Face(m_Face);

				//FT_Done_FreeType(m_FreeTypeLib);
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

			void FontGeneratorBase::GetGlyphBoundingSize(Vector2F& Size)
			{
				FT_BBox bb;
				FT_Outline_Get_CBox(&m_Face->glyph->outline, &bb);

				Size.X = (bb.xMax - bb.xMin);
				Size.Y = (bb.yMax - bb.yMin);
			}
		}
	}
}