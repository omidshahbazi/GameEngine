// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include <WindowUtility\Window.h>

namespace Engine
{
	using namespace WindowUtility;
	using namespace Containers;

	namespace Rendering
	{
		class RENDERING_API RenderWindow : public Window
		{
		public:
			RenderWindow(const String& Name);
			virtual ~RenderWindow(void)
			{
			}

			void SetIsFullscreen(bool Value);
		};
	}
}

#endif