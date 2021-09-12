// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <TTFParser\TTFParser.h>
#include <TTFParser\Private\MeshFontGenerator.h>
#include <TTFParser\Private\TextureFontGenerator.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		using namespace Private;

		void TTFParser::Parse(const ByteBuffer& Buffer, FontInfo& FontInfo)
		{
			if (FontInfo.RenderType == FontRenderTypes::Mesh)
			{
				MeshFontGenerator loader(Buffer);
				loader.Generate(FontInfo);
			}
			else if (FontInfo.RenderType == FontRenderTypes::Texture)
			{
				TextureFontGenerator loader(Buffer);
				loader.Generate(FontInfo);
			}
		}
	}
}