// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\Window.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	namespace Utility
	{
		Window::Window(const String& Name) :
			m_Handle(0),
			m_Name(Name),
			m_Style(PlatformWindow::Styles::Overlapped | PlatformWindow::Styles::Caption | PlatformWindow::Styles::SystemMenu | PlatformWindow::Styles::ThickFrame | PlatformWindow::Styles::MinimizeBox | PlatformWindow::Styles::MaximizeBox),
			m_ExtraStyle(PlatformWindow::ExtraStyles::OverlappedWindow),
			m_State(States::Noraml),
			m_BorderStyle(BorderStyles::Normal),
			m_SizableMode(SizableModes::Sizable),
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
			m_Handle = PlatformWindow::Create(PlatformOS::GetExecutingModuleInstance(), m_Name.GetValue(), m_Style, [&](PlatformWindow::WindowMessages Message) { return MessageProcedure(Message); });

			UpdateStyle();

			return false;
		}

		void Window::SetTitle(const String& Value)
		{
			Assert(m_Handle != 0, "Window doesn't initialized");

			m_Title = Value;

			PlatformWindow::SetTitle(m_Handle, Value.GetValue());
		}

		void Window::SetPosition(const Vector2I& Value)
		{
			Assert(m_Handle != 0, "Window doesn't initialized");

			m_Position = Value;

			PlatformWindow::SetPosition(m_Handle, m_Position.X, m_Position.Y);
		}

		void Window::SetSize(const Vector2I& Value)
		{
			Assert(m_Handle != 0, "Window doesn't initialized");

			m_Size = Value;

			PlatformWindow::SetSize(m_Handle, m_Size.X, m_Size.Y);

			uint16 x, y;
			PlatformWindow::GetClientSize(m_Handle, x, y);
			m_ClientSize.X = x;
			m_ClientSize.Y = y;
		}

		bool Window::GetIsVisible(void) const
		{
			return ((m_Style & PlatformWindow::Styles::Visible) == PlatformWindow::Styles::Visible);
		}

		void Window::SetIsVisible(bool Value)
		{
			if (Value)
				m_Style |= PlatformWindow::Styles::Visible;
			else
				m_Style &= ~PlatformWindow::Styles::Visible;

			UpdateStyle();
		}

		bool Window::GetIsDisabled(void) const
		{
			return ((m_Style & PlatformWindow::Styles::Disabled) == PlatformWindow::Styles::Disabled);
		}

		void Window::SetIsDisabled(bool Value)
		{
			if (Value)
				m_Style |= PlatformWindow::Styles::Disabled;
			else
				m_Style &= ~PlatformWindow::Styles::Disabled;

			UpdateStyle();
		}

		bool Window::GetShowCaption(void) const
		{
			return ((m_Style & PlatformWindow::Styles::Caption) == PlatformWindow::Styles::Caption);
		}

		void Window::SetShowCaption(bool Value)
		{
			if (Value)
				m_Style |= PlatformWindow::Styles::Caption;
			else
				m_Style &= ~PlatformWindow::Styles::Caption;

			UpdateStyle();
		}

		bool Window::GetSystemMenu(void) const
		{
			return ((m_Style & PlatformWindow::Styles::SystemMenu) == PlatformWindow::Styles::SystemMenu);
		}

		void Window::SetSystemMenu(bool Value)
		{
			if (Value)
				m_Style |= PlatformWindow::Styles::SystemMenu;
			else
				m_Style &= ~PlatformWindow::Styles::SystemMenu;

			UpdateStyle();
		}

		bool Window::GetShowBorder(void) const
		{
			return ((m_Style & PlatformWindow::Styles::Border) == PlatformWindow::Styles::Border);
		}

		void Window::SetShowBorder(bool Value)
		{
			if (Value)
				m_Style |= PlatformWindow::Styles::Border;
			else
				m_Style &= ~PlatformWindow::Styles::Border;

			UpdateStyle();
		}

		bool Window::GetIsTabStop(void) const
		{
			return ((m_Style & PlatformWindow::Styles::TabStop) == PlatformWindow::Styles::TabStop);
		}

		void Window::SetIsTabStop(bool Value)
		{
			if (Value)
				m_Style |= PlatformWindow::Styles::TabStop;
			else
				m_Style &= ~PlatformWindow::Styles::TabStop;

			UpdateStyle();
		}

		bool Window::GetShowMinimizeBox(void) const
		{
			return ((m_Style & PlatformWindow::Styles::MinimizeBox) == PlatformWindow::Styles::MinimizeBox);
		}

		void Window::SetShowMinimizeBox(bool Value)
		{
			if (Value)
				m_Style |= PlatformWindow::Styles::MinimizeBox;
			else
				m_Style &= ~PlatformWindow::Styles::MinimizeBox;

			UpdateStyle();
		}

		bool Window::GetShowMaximizeBox(void) const
		{
			return ((m_Style & PlatformWindow::Styles::MaximizeBox) == PlatformWindow::Styles::MaximizeBox);
		}

		void Window::SetShowMaximizeBox(bool Value)
		{
			if (Value)
				m_Style |= PlatformWindow::Styles::MaximizeBox;
			else
				m_Style &= ~PlatformWindow::Styles::MaximizeBox;

			UpdateStyle();
		}

		bool Window::GetIsTopMost(void) const
		{
			return ((m_ExtraStyle & PlatformWindow::ExtraStyles::TopMost) == PlatformWindow::ExtraStyles::TopMost);
		}

		void Window::SetIsTopMost(bool Value)
		{
			if (Value)
				m_ExtraStyle |= PlatformWindow::ExtraStyles::TopMost;
			else
				m_ExtraStyle &= ~PlatformWindow::ExtraStyles::TopMost;

			UpdateStyle();
		}

		bool Window::GetAcceptFiles(void) const
		{
			return ((m_ExtraStyle & PlatformWindow::ExtraStyles::AcceptFiles) == PlatformWindow::ExtraStyles::AcceptFiles);
		}

		void Window::SetAcceptFiles(bool Value)
		{
			if (Value)
				m_ExtraStyle |= PlatformWindow::ExtraStyles::AcceptFiles;
			else
				m_ExtraStyle &= ~PlatformWindow::ExtraStyles::AcceptFiles;

			UpdateStyle();
		}

		void Window::SetState(States Value)
		{
			m_State = Value;

			m_Style &= ~PlatformWindow::Styles::Minimize;
			m_Style &= ~PlatformWindow::Styles::Maximize;

			switch (Value)
			{
			case States::Minimized:
				m_Style |= PlatformWindow::Styles::Minimize;
				break;

			case States::Maximized:
				m_Style |= PlatformWindow::Styles::Maximize;
				break;
			}

			UpdateStyle();
		}

		void Window::SetBorderStyle(BorderStyles Value)
		{
			m_BorderStyle = Value;

			m_Style &= ~PlatformWindow::Styles::ThickFrame;
			m_Style &= ~PlatformWindow::Styles::DialogFrame;
			m_ExtraStyle &= ~PlatformWindow::ExtraStyles::ToolWindow;

			switch (Value)
			{
			case BorderStyles::Normal:
				m_Style |= PlatformWindow::Styles::DialogFrame;
				break;

			case BorderStyles::Tool:
				m_Style |= PlatformWindow::Styles::ThickFrame;
				m_ExtraStyle |= PlatformWindow::ExtraStyles::ToolWindow;
				break;
			}
		}

		void Window::SetSizableMode(SizableModes Value)
		{
			m_SizableMode = Value;

			switch (Value)
			{
			case SizableModes::Sizable:
				break;
			case SizableModes::Fixed:
				break;
			}
		}

		bool Window::MessageProcedure(PlatformWindow::WindowMessages Message)
		{
			switch (Message)
			{
			case PlatformWindow::WindowMessages::Create:
				break;
			case PlatformWindow::WindowMessages::Size:
			{
				uint16 x;
				uint16 y;

				PlatformWindow::GetSize(m_Handle, x, y);

				bool shouldUpdateSize = false;

				if (x < m_MinimumSize.X)
				{
					x = m_MinimumSize.X;
					shouldUpdateSize = true;
				}
				if (x > m_MaximumSize.X)
				{
					x = m_MaximumSize.X;
					shouldUpdateSize = true;
				}

				if (y < m_MinimumSize.Y)
				{
					y = m_MinimumSize.Y;
					shouldUpdateSize = true;
				}
				if (y > m_MaximumSize.Y)
				{
					y = m_MaximumSize.Y;
					shouldUpdateSize = true;
				}

				if (shouldUpdateSize)
					PlatformWindow::SetSize(m_Handle, x, y);

				if (x != m_Size.X || y != m_Size.Y)
				{
					m_Size.X = x;
					m_Size.Y = y;

					PlatformWindow::GetClientSize(m_Handle, x, y);
					m_ClientSize.X = x;
					m_ClientSize.Y = y;

					for each (auto listener in m_Listeners)
						listener->OnWindowResized(this);
				}
			} break;
			case PlatformWindow::WindowMessages::Move:
			{
				int16 x;
				int16 y;

				PlatformWindow::GetClientPosition(m_Handle, x, y);
				if (x != m_ClientPosition.X || y != m_ClientPosition.Y)
				{
					m_ClientPosition.X = x;
					m_ClientPosition.Y = y;

					PlatformWindow::GetPosition(m_Handle, x, y);
					m_Position.X = x;
					m_Position.Y = y;

					for each (auto listener in m_Listeners)
						listener->OnWindowPositioned(this);
				}
			} break;
			case PlatformWindow::WindowMessages::Close:
				m_ShouldClose = true;
				break;
			}

			return false;
		}

		void Window::UpdateStyle(void)
		{
			Assert(m_Handle != 0, "Window doesn't initialized");

			PlatformWindow::SetStyle(m_Handle, m_Style);

			PlatformWindow::SetExtraStyle(m_Handle, m_ExtraStyle);
		}
	}
}