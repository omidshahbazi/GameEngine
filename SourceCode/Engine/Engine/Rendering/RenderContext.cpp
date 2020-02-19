// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\RenderContext.h>

namespace Engine
{
	namespace Rendering
	{
		RenderContext::RenderContext(PlatformWindow::WindowHandle WindowHandle) :
			m_WindowHandle(WindowHandle),
			m_IsActive(false)
		{
		}
	}
}