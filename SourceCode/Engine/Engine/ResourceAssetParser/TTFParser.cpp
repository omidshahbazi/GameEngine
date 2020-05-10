// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\TTFParser.h>
#include <ResourceAssetParser\Private\MeshFontGenerator.h>
#include <ResourceAssetParser\Private\TextureFontGenerator.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		using namespace Private;

		void TTFParser::Parse(const ByteBuffer& Buffer, FontInfo& FontInfo)
		{
			if (FontInfo.RenderType == Font::RenderTypes::Mesh)
			{
				MeshFontGenerator loader(Buffer);

				loader.GetGlyphCount();

				loader.Generate(FontInfo);
			}
			else if (FontInfo.RenderType == Font::RenderTypes::Texture)
			{
				//TextureFontLoader loader;
				//loader.Load(Buffer);
			}
		}
	}
}