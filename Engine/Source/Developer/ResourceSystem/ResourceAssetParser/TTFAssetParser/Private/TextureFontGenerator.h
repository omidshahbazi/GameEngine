// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_FONT_GENERATOR_H
#define TEXTURE_FONT_GENERATOR_H

#include <FontSystem\FontInfo.h>
#include <TTFAssetParser\Private\FontGeneratorBase.h>

namespace Engine
{
	using namespace Containers;
	using namespace FontSystem;

	namespace ResourceAssetParser
	{
		namespace Private
		{
			class TextureFontGenerator : public FontGeneratorBase
			{
			public:
				TextureFontGenerator(const ByteBuffer& TTFBuffer);

				void Generate(FontInfo& FontInfo) override;
			};
		}
	}
}

#endif