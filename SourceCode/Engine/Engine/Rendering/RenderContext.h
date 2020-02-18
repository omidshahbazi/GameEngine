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

			virtual void Activate(void)
			{
				m_IsActive = true;
			}

			virtual void Deactivate(void)
			{
				m_IsActive = false;
			}

			PlatformWindow::WindowHandle GetWindowHandler(void) const
			{
				return m_WindowHandle;
			}

			bool GetIsActive(void) const
			{
				return m_IsActive;
			}

		private:
			PlatformWindow::WindowHandle m_WindowHandle;
			bool m_IsActive;
		};
	}
}

#endif