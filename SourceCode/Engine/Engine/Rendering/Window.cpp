// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Window.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		bool Window::ShouldClose(void) const
		{
			return m_Device->WindowShouldClose(m_Handle);
		}
	}
}