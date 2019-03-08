// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\Window.h>

namespace Engine
{
	namespace Utility
	{
		Window::Window(const String & Name) :
			m_Handle(0),
			m_Name(Name),
			m_ShouldClose(false)
		{
		}

		Window::~Window(void)
		{
			if (m_Handle != 0)
				PlatformWindow::Destroy(m_Handle);
		}

		bool Window::Initialize(void)
		{
			m_Handle = PlatformWindow::Create(PlatformOS::GetExecutingModuleInstance(), m_Name.GetValue(), PlatformWindow::Styles::OverlappedWindow, MessageProcedure);

			return false;
		}

		void Window::SetTitle(const String & Title)
		{
			m_Title = Title;

			PlatformWindow::SetTitle(m_Handle, Title.GetValue());
		}

		void Window::SetSize(uint16 Width, uint16 Height)
		{
			m_Width = Width;
			m_Height = Height;

			PlatformWindow::SetSize(m_Handle, m_Width, m_Height);
		}

		bool Window::ShouldClose(void) const
		{
			return false;
		}

		int32 Window::MessageProcedure(PlatformWindow::WindowHandle Handle, uint32 Message, uint32 * WParam, uint32 * LParam)
		{
			if (Message == 0x0012)
				m_ShouldClose = true;

			return 0;
		}
	}
}