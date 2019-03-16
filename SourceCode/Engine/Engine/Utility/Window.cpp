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
			m_Handle = PlatformWindow::Create(PlatformOS::GetExecutingModuleInstance(), m_Name.GetValue(), PlatformWindow::Styles::OverlappedWindow, [&](PlatformWindow::WindowMessages Message) { return MessageProcedure(Message); });

			return false;
		}

		void Window::SetTitle(const String & Value)
		{
			m_Title = Value;

			PlatformWindow::SetTitle(m_Handle, Value.GetValue());
		}

		void Window::SetSize(const Vector2I &Value)
		{
			m_Size = Value;

			PlatformWindow::SetSize(m_Handle, m_Size.X, m_Size.Y);
		}

		bool Window::MessageProcedure(PlatformWindow::WindowMessages Message)
		{
			switch (Message)
			{
			case PlatformWindow::WindowMessages::Create:
				break;
			case PlatformWindow::WindowMessages::Size:
			{
				uint16 width;
				uint16 height;
				PlatformWindow::GetClientSize(m_Handle, width, height);

				if (width != m_Size.X || height != m_Size.Y)
				{
					m_Size = { width, height };

					for each (auto listener in m_Listeners)
						listener->OnWindowResized(this);
				}
			} break;
			case PlatformWindow::WindowMessages::Close:
				m_ShouldClose = true;
				break;
			}

			return false;
		}
	}
}