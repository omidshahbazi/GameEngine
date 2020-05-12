// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_INFO_H
#define FONT_INFO_H

#include <Containers\Vector.h>
#include <FontSystem\Font.h>
#include <Rendering\MeshInfo.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace FontSystem
	{
		struct FontInfo
		{
		public:
			struct GlyphInfo
			{
			public:
				GlyphInfo(void) :
					CharCode(0)
				{
				}
				GlyphInfo(const GlyphInfo& Other)
				{
					*this = Other;
				}
				~GlyphInfo(void)
				{
				}

				GlyphInfo& operator =(const GlyphInfo& Other)
				{
					CharCode = Other.CharCode;
					
					Size = Other.Size;
					Bearing = Other.Bearing;
					Advance = Other.Advance;

					MeshInfo = Other.MeshInfo;

					return *this;
				}

			public:
				uint64 CharCode;

				Vector2F Size;
				Vector2F Bearing;
				Vector2F Advance;

				union
				{
				public:
					MeshInfo MeshInfo;
				};
			};

			typedef Vector<GlyphInfo> GlyphList;

		public:
			FontInfo(void)
			{
			}

			~FontInfo(void)
			{
			}

		public:
			Font::RenderTypes RenderType;
			GlyphList Glyphs;
		};
	}
}

#endif