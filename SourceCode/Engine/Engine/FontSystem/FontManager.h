// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <FontSystem\FontInfo.h>

namespace Engine
{
	namespace FontSystem
	{
		class Font;

		class FONTSYSTEM_API FontManager
		{
		private:
			SINGLETON_DECLARATION(FontManager)

		private:
			FontManager(void);
			~FontManager(void);

		public:
			Font* CreateFont(const FontInfo* Buffer);
			void DestroyFont(Font* Font);
		};
	}
}

#endif