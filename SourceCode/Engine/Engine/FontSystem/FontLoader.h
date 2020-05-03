// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_GENERATOR_H
#define FONT_GENERATOR_H

#include <FontSystem\Font.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;

	namespace FontSystem
	{
		class FONTSYSTEM_API FontLoader
		{
		public:
			static void CreateFontFromTrueTypeFont(const ByteBuffer& InBuffer, ByteBuffer& OutBuffer, Font::RenderTypes RenderType);

			static void LoadFont(const ByteBuffer& Buffer, Font* Font);
		};
	}
}

#endif