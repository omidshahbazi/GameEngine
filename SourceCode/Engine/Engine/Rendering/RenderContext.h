// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <Utility\Window.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		class RENDERING_API RenderContext
		{
		public:
			virtual ~RenderContext(void) {}

			PlatformWindow::WindowHandle GetWindowHandler(void) const
			{
				return m_WindowHandle;
			}

		private:
			PlatformWindow::WindowHandle m_WindowHandle;
		};
	}
}

#endif