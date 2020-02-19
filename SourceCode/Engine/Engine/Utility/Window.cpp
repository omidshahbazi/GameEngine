// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\Window.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	namespace Utility
	{
#define IS_STYLE_SET(Style) ((PlatformWindow::GetStyle(m_Handle) & Style) == Style)
#define SET_STYLE_STATE(Style, Enabled) \
		{ \
			PlatformWindow::Styles style = PlatformWindow::GetStyle(m_Handle); \
			if (Enabled) \
				style |= Style; \
			else \
				style &= ~Style; \
			PlatformWindow::SetStyle(m_Handle, style); \
		}

#define IS_EXTRA_STYLE_SET(Style) ((PlatformWindow::GetExtraStyle(m_Handle) & Style) == Style)
#define SET_EXTRA_STYLE_STATE(Style, Enabled) \
		{ \
			PlatformWindow::ExtraStyles style = PlatformWindow::GetExtraStyle(m_Handle); \
			if (Enabled) \
				style |= Style; \
			else \
				style &= ~Style; \
			PlatformWindow::SetExtraStyle(m_Handle, style); \
		}

		Window::Window(const String& Name) :
			m_Handle(0),
			m_Name(Name),
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
			PlatformWindow::Styles style = PlatformWindow::Styles::Overlapped | PlatformWindow::Styles::Caption | PlatformWindow::Styles::SystemMenu | PlatformWindow::Styles::ThickFrame | PlatformWindow::Styles::MinimizeBox | PlatformWindow::Styles::MaximizeBox;

			m_Handle = PlatformWindow::Create(PlatformOS::GetExecutingModuleInstance(), m_Name.GetValue(), style, [&](PlatformWindow::WindowMessages Message, void* Parameter) { return MessageProcedure(Message, Parameter); });
			
			SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::OverlappedWindow, true);

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
			return IS_STYLE_SET(PlatformWindow::Styles::Visible);
		}

		void Window::SetIsVisible(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::Visible, Value);
		}

		bool Window::GetIsDisabled(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::Disabled);
		}

		void Window::SetIsDisabled(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::Disabled, Value);
		}

		bool Window::GetShowCaption(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::Caption);
		}

		void Window::SetShowCaption(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::Caption, Value);
		}

		bool Window::GetSystemMenu(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::SystemMenu);
		}

		void Window::SetSystemMenu(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::SystemMenu, Value);
		}

		bool Window::GetShowBorder(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::Border);
		}

		void Window::SetShowBorder(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::Border, Value);
		}

		bool Window::GetIsTabStop(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::TabStop);
		}

		void Window::SetIsTabStop(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::TabStop, Value);
		}

		bool Window::GetShowMinimizeBox(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::MinimizeBox);
		}

		void Window::SetShowMinimizeBox(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::MinimizeBox, Value);
		}

		bool Window::GetShowMaximizeBox(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::MaximizeBox);
		}

		void Window::SetShowMaximizeBox(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::MaximizeBox, Value);
		}

		bool Window::GetIsTopMost(void) const
		{
			return IS_EXTRA_STYLE_SET(PlatformWindow::ExtraStyles::TopMost);
		}

		void Window::SetIsTopMost(bool Value)
		{
			SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::TopMost, Value);
		}

		bool Window::GetAcceptFiles(void) const
		{
			return IS_EXTRA_STYLE_SET(PlatformWindow::ExtraStyles::AcceptFiles);
		}

		void Window::SetAcceptFiles(bool Value)
		{
			SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::AcceptFiles, Value);
		}

		void Window::SetState(States Value)
		{
			m_State = Value;

			SET_STYLE_STATE(PlatformWindow::Styles::Minimize, false);
			SET_STYLE_STATE(PlatformWindow::Styles::Maximize, false);

			switch (Value)
			{
			case States::Minimized:
				SET_STYLE_STATE(PlatformWindow::Styles::Minimize, true);
				break;

			case States::Maximized:
				SET_STYLE_STATE(PlatformWindow::Styles::Maximize, true);
				break;
			}
		}

		void Window::SetBorderStyle(BorderStyles Value)
		{
			m_BorderStyle = Value;

			SET_STYLE_STATE(PlatformWindow::Styles::DialogFrame, false);
			SET_STYLE_STATE(PlatformWindow::Styles::ThickFrame, false);
			SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::ToolWindow, false);

			switch (Value)
			{
			case BorderStyles::Normal:
				SET_STYLE_STATE(PlatformWindow::Styles::DialogFrame, true);
				break;

			case BorderStyles::Tool:
				SET_STYLE_STATE(PlatformWindow::Styles::ThickFrame, true);
				SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::ToolWindow, true);
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

		bool Window::MessageProcedure(PlatformWindow::WindowMessages Message, void* Parameter)
		{
			switch (Message)
			{
			case PlatformWindow::WindowMessages::Create:
				break;
			case PlatformWindow::WindowMessages::Resized:
			{
				uint16 x;
				uint16 y;

				PlatformWindow::GetSize(m_Handle, x, y);
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
			case PlatformWindow::WindowMessages::Moved:
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
			case PlatformWindow::WindowMessages::GetMinMaxInfo:
			{
				PlatformWindow::MinMaxSizeInfo* info = ReinterpretCast(PlatformWindow::MinMaxSizeInfo*, Parameter);

				info->MinWidth = m_MinimumSize.X;
				info->MinHeight = m_MinimumSize.Y;
				info->MaxWidth = m_MaximumSize.X;
				info->MaxHeight = m_MaximumSize.Y;

				return true;
			} break;
			}

			return false;
		}

		void Window::UpdateStyle(void)
		{
			Assert(m_Handle != 0, "Window doesn't initialized");

			//PlatformWindow::SetStyle(m_Handle, m_Style);

			//PlatformWindow::SetExtraStyle(m_Handle, m_ExtraStyle);
		}
	}
}