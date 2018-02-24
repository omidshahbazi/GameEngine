// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Window.h>

namespace Engine
{
	namespace Rendering
	{
		Window::Window(IDevice *Device, Handle Handle) :
			m_Device(Device),
			m_Handle(Handle)
		{
		}

		Window::~Window(void)
		{
		}
	}
}