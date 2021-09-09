// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_INFO_H
#define FONT_INFO_H

#include <Containers\List.h>
#include <FontSystem\Font.h>
#include <RenderDevice\MeshInfo.h>
#include <RenderDevice\TextureInfo.h>

namespace Engine
{
	using namespace Containers;
	using namespace RenderDevice;

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

					Bounds = Other.Bounds;

					return *this;
				}

			public:
				uint64 CharCode;

				Vector2F Size;
				Vector2F Bearing;
				Vector2F Advance;

				MeshInfo MeshInfo;

				Vector4F Bounds;
			};

			typedef List<GlyphInfo> GlyphList;

		public:
			FontInfo(void) :
				Size(0),
				RenderType(Font::RenderTypes::Mesh)
			{
			}

			~FontInfo(void)
			{
			}

		public:
			float32 Size;
			Font::RenderTypes RenderType;
			GlyphList Glyphs;
			TextureInfo TextureInfo;
		};
	}
}

#endif