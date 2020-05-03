// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_FONT_LOADER_H
#define I_FONT_LOADER_H

#include <Containers\Buffer.h>
#include <MathContainers\MathContainers.h>

#include <FreeType\include\ft2build.h>
#include FT_FREETYPE_H
#include <FontSystem\Private\src\FTVectoriser.h>

namespace Engine
{
	using namespace Containers;
	using namespace MathContainers;

	namespace FontSystem
	{
		namespace Private
		{
			class FontLoaderBase
			{
			public:
				FontLoaderBase(void);

				virtual	~FontLoaderBase(void);

				virtual	void Load(const ByteBuffer& InBuffer, ByteBuffer& OutBuffer) = 0;

			protected:
				virtual FT_Face LoadFace(const ByteBuffer& InBuffer);

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