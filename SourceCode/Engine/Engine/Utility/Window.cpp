// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\Window.h>

namespace Engine
{
	namespace Utility
	{
		Window::Window(const String & Name) :
			m_Handle(0),
			m_Name(Name)
		{
		}

		Window::~Window(void)
		{
			if (m_Handle != 0)
				PlatformWindow::Destroy(m_Handle);
		}

		bool Window::Initialize(void)
		{
			m_Handle = PlatformWindow::Create(PlatformOS::GetExecutingModuleInstance(), m_Name.GetValue(), PlatformWindow::Styles::OverlappedWindow, PlatformWindow::DefaultProcedure);

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
	}
}