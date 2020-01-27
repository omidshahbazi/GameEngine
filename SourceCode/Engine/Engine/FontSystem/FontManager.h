// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <Containers\Buffer.h>

namespace Engine
{
	using namespace Containers;

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
			Font * LoadFont(const ByteBuffer &Data);
			void DestroyFont(Font *Font);
		};
	}
}

#endif