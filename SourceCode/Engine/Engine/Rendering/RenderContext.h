// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <Platform\PlatformWindow.h>

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		class RENDERING_API RenderContext
		{
		public:
			RenderContext(PlatformWindow::WindowHandle WindowHandle);

			virtual ~RenderContext(void) {}

			virtual void ResetState(void) = 0;

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