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
			PlatformWindow::Update(m_Handle); \
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
			PlatformWindow::Update(m_Handle); \
		}

		Window::Window(const String& Name) :
			m_Handle(0),
			m_Name(Name),
			m_State(States::Noraml),
			m_BorderStyle(BorderStyles::Normal),
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

			PlatformWindow::SetSize(m_Handle, Value.X, Value.Y);
		}

		bool Window::GetIsVisible(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::Visible);
		}

		void Window::SetIsVisible(bool Value)
		{
			PlatformWindow::ShowWindow(m_Handle, (Value ? PlatformWindow::ShowWindowStates::Show : PlatformWindow::ShowWindowStates::Hide));
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

		bool Window::GetShowSystemMenu(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::SystemMenu);
		}

		void Window::SetShowSystemMenu(bool Value)
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

		bool Window::GetShowFrame(void) const
		{
			return IS_STYLE_SET(PlatformWindow::Styles::Overlapped) && IS_STYLE_SET(PlatformWindow::Styles::ThickFrame);
		}

		void Window::SetShowFrame(bool Value)
		{
			SET_STYLE_STATE(PlatformWindow::Styles::Overlapped, Value);
			SET_STYLE_STATE(PlatformWindow::Styles::ThickFrame, Value);

			UpdateSize(true);
		}

		bool Window::GetIsTopMost(void) const
		{
			return IS_EXTRA_STYLE_SET(PlatformWindow::ExtraStyles::TopMost);
		}

		void Window::SetIsTopMost(bool Value)
		{
			SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::TopMost, Value);
			PlatformWindow::SetPositionOption(m_Handle, Value ? PlatformWindow::Places::TopMost : PlatformWindow::Places::NoTopMost);
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

			switch (Value)
			{
			case States::Noraml:
				PlatformWindow::ShowWindow(m_Handle, PlatformWindow::ShowWindowStates::ShowNormal);
				break;

			case States::Minimized:
				PlatformWindow::ShowWindow(m_Handle, PlatformWindow::ShowWindowStates::ShowMinimized);
				break;

			case States::Maximized:
				PlatformWindow::ShowWindow(m_Handle, PlatformWindow::ShowWindowStates::Maximize);
				break;
			}
		}

		void Window::SetBorderStyle(BorderStyles Value)
		{
			m_BorderStyle = Value;

			switch (Value)
			{
			case BorderStyles::Normal:
				SET_STYLE_STATE(PlatformWindow::Styles::ThickFrame, false);
				SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::ToolWindow, false);
				SET_STYLE_STATE(PlatformWindow::Styles::DialogFrame, true);
				SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::DialogModalFrame, true);
				break;

			case BorderStyles::Tool:
				SET_STYLE_STATE(PlatformWindow::Styles::DialogFrame, false);
				SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::DialogModalFrame, false);
				SET_STYLE_STATE(PlatformWindow::Styles::ThickFrame, true);
				SET_EXTRA_STYLE_STATE(PlatformWindow::ExtraStyles::ToolWindow, true);
				break;
			}
		}

		void Window::UpdateSize(bool Force)
		{
			uint16 x;
			uint16 y;

			PlatformWindow::GetSize(m_Handle, x, y);
			if (Force || x != m_Size.X || y != m_Size.Y)
			{
				m_Size.X = x;
				m_Size.Y = y;

				if (GetShowFrame())
					PlatformWindow::GetClientSize(m_Handle, x, y);

				m_ClientSize.X = x;
				m_ClientSize.Y = y;

				CALL_CALLBACK(IListener, OnSizeChanged, this)
			}
		}

		bool Window::MessageProcedure(PlatformWindow::WindowMessages Message, void* Parameter)
		{
			switch (Message)
			{
			case PlatformWindow::WindowMessages::Create:
			{

			} break;
			case PlatformWindow::WindowMessages::Resized:
			{
				UpdateSize(false);
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

					CALL_CALLBACK(IListener, OnPositionChanged, this)
				}
			} break;
			case PlatformWindow::WindowMessages::GetMinMaxInfo:
			{
				PlatformWindow::MinMaxSizeInfo* info = ReinterpretCast(PlatformWindow::MinMaxSizeInfo*, Parameter);

				info->MinWidth = m_MinimumSize.X;
				info->MinHeight = m_MinimumSize.Y;
				info->MaxWidth = m_MaximumSize.X;
				info->MaxHeight = m_MaximumSize.Y;

				return true;
			} break;
			case PlatformWindow::WindowMessages::KeyUp:
			{
				PlatformWindow::KeyInfo* info = ReinterpretCast(PlatformWindow::KeyInfo*, Parameter);

				for (uint16 i = 0; i < info->RepeatCount; ++i)
					CALL_CALLBACK(IListener, OnKeyUp, this, info->Key)
			} break;
			case PlatformWindow::WindowMessages::KeyDown:
			{
				PlatformWindow::KeyInfo* info = ReinterpretCast(PlatformWindow::KeyInfo*, Parameter);

				CALL_CALLBACK(IListener, OnKeyDown, this, info->Key)

					CALL_CALLBACK(IListener, OnKeyPressed, this, info->Key)
			} break;
			case PlatformWindow::WindowMessages::MouseDown:
			{
				PlatformWindow::MouseInfo* info = ReinterpretCast(PlatformWindow::MouseInfo*, Parameter);

				CALL_CALLBACK(IListener, OnMouseDown, this, info->Key, Vector2I(info->X, info->Y))
			} break;
			case PlatformWindow::WindowMessages::MouseUp:
			{
				PlatformWindow::MouseInfo* info = ReinterpretCast(PlatformWindow::MouseInfo*, Parameter);

				CALL_CALLBACK(IListener, OnMouseUp, this, info->Key, Vector2I(info->X, info->Y))

					CALL_CALLBACK(IListener, OnMouseClick, this, info->Key, Vector2I(info->X, info->Y))
			} break;
			case PlatformWindow::WindowMessages::MouseWheel:
			{
				PlatformWindow::MouseInfo* info = ReinterpretCast(PlatformWindow::MouseInfo*, Parameter);

				CALL_CALLBACK(IListener, OnMouseWheel, this, Vector2I(info->X, info->Y), info->WheelDelta)
			} break;
			case PlatformWindow::WindowMessages::MouseMove:
			{
				PlatformWindow::MouseInfo* info = ReinterpretCast(PlatformWindow::MouseInfo*, Parameter);

				CALL_CALLBACK(IListener, OnMouseMove, this, Vector2I(info->X, info->Y))
			} break;
			case PlatformWindow::WindowMessages::MouseLeave:
			{
				CALL_CALLBACK(IListener, OnMouseLeave, this)
			} break;
			case PlatformWindow::WindowMessages::CalculateNonClientSize:
			{
				//PlatformWindow::Rect* rect = ReinterpretCast(PlatformWindow::Rect*, Parameter);
				if (!GetShowFrame())
					return true;
			} break;
			case PlatformWindow::WindowMessages::Close:
			{
				m_ShouldClose = true;

				CALL_CALLBACK(IListener, OnClosing, this)
			} break;
			}

			return false;
		}
	}
}