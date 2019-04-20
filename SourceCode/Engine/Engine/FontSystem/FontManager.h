// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <Containers\Strings.h>

namespace Engine
{
	namespace FontSystem
	{
		class FONTSYSTEM_API FontManager
		{
		private:
			SINGLETON_DECLARATION(FontManager)

		private:
			FontManager(void);
			~FontManager(void);

		};
	}
}

#endif