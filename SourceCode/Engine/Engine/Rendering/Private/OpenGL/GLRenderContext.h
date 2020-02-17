// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GL_RENDER_CONTEXT_H
#define GL_RENDER_CONTEXT_H

#include <Rendering\RenderContext.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				class RENDERING_API GLRenderContext : public RenderContext
				{
				public:
					GLRenderContext(PlatformWindow::WindowHandle WindowHandle, PlatformWindow::ContextHandle ContextHandle, PlatformWindow::WGLContextHandle WGLContextHandle) :
						RenderContext(WindowHandle),
						m_ContextHandle(ContextHandle),
						m_WGLContextHandle(WGLContextHandle)
					{
					}

					PlatformWindow::ContextHandle GetContextHandle(void) const
					{
						return m_ContextHandle;
					}

					PlatformWindow::WGLContextHandle GetWGLContextHandle(void) const
					{
						return m_WGLContextHandle;
					}

				private:
					PlatformWindow::ContextHandle m_ContextHandle;
					PlatformWindow::WGLContextHandle m_WGLContextHandle;
				};
			}
		}
	}
}

#endif